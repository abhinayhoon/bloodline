#include "bloodline/Account.hpp"
#include "bloodline/Donor.hpp"
#include "bloodline/Organization.hpp"
#include "bloodline/Camp.hpp"
#include "bloodline/Donation.hpp"
#include "bloodline/BloodUnit.hpp"
#include "bloodline/Request.hpp"
#include "bloodline/MatchingEngine.hpp"
#include "bloodline/NotificationService.hpp"
#include "bloodline/AuditLogger.hpp"
#include "bloodline/AccountFactory.hpp"
#include "bloodline/Compatibility.hpp"
#include "bloodline/EligibilityRules.hpp"
#include "bloodline/FileRepository.hpp"
#include "bloodline/SessionManager.hpp"
#include "bloodline/Errors.hpp"
#include "bloodline/BloodGroup.hpp"
#include "bloodline/DateUtil.hpp"

#include <httplib.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <string>
#include <memory>
#include <filesystem>
#include <algorithm>
#include <map>
#include <sstream>
#include <regex>

using json = nlohmann::json;
namespace bl = bloodline;

// ───────────────────────────────────────────────
// Application state: repositories, sessions, engine, services
// ───────────────────────────────────────────────

struct AppState {
    bl::AccountFileRepository accounts;
    bl::DonorFileRepository donors;
    bl::CampFileRepository camps;
    bl::DonationFileRepository donations;
    bl::RequestFileRepository requests;
    bl::SessionManager sessions;
    bl::EligibilityEngine eligibility;
    bl::CompatibilityTable compatibility;
    bl::AuditLogger audit;
    bl::LogNotificationService notifications;
    bl::RateLimiter rateLimiter;
    bl::DonorMatchingEngine matchingEngine;

    AppState(const std::string& dataDir)
        : accounts(dataDir + "/accounts.csv"),
          donors(dataDir + "/donors.csv"),
          camps(dataDir + "/camps.csv"),
          donations(dataDir + "/donations.csv"),
          requests(dataDir + "/requests.csv"),
          audit(dataDir + "/audit_log.csv"),
          notifications(dataDir + "/notifications.log"),
          rateLimiter(5, 300) {}

    void loadAll() {
        accounts.load();
        donors.load();
        camps.load();
        donations.load();
        requests.load();
        syncDonorDonationHistory();
    }

    void syncDonorDonationHistory() {
        // Sync donation counts and last donation date from donations repository into donors
        std::map<std::string, std::pair<int, std::string>> donorStats;
        for (const auto& don : donations.all()) {
            if (don.status() == "Completed" || don.status() == "Transfused" || don.status() == "Discarded") {
                auto& stat = donorStats[don.donorUsername()];
                stat.first++;
                if (stat.second.empty() || don.date() > stat.second) {
                    stat.second = don.date();
                }
            }
        }
        for (auto& d : donors.all()) {
            auto it = donorStats.find(d.username());
            if (it != donorStats.end()) {
                d.setDonationCount(it->second.first);
                d.setLastDonationDate(it->second.second);
            }
        }
        donors.save();
    }
};

// ───────────────────────────────────────────────
// Seed data: initial accounts, camps, and records
// ───────────────────────────────────────────────

void seedData(AppState& state) {
    auto makeCred = [](const std::string& pw) {
        return bl::Credential::fromPassword(pw);
    };

    // Seed Admin account
    if (!state.accounts.findByUsername("admin")) {
        std::cout << "[seed] Creating default admin account...\n";
        state.accounts.add(std::make_unique<bl::Admin>(
            "admin", "BloodLine Administrator", makeCred("admin123")));
        state.accounts.save();
    }

    // Seed requests if empty
    if (state.requests.all().empty()) {
        std::cout << "[seed] Creating default blood requests...\n";
        state.requests.add(std::make_unique<bl::UrgentRequest>(
            "REQ-101", "ngo_admin", "RamSay Hospital, Nainital",
            bl::BloodGroup::ONeg, 3, true, "2026-10-10", bl::RequestStatus::Open));
        state.requests.add(std::make_unique<bl::UrgentRequest>(
            "REQ-102", "ngo_admin", "BD Pandey District Hospital",
            bl::BloodGroup::APos, 2, false, "2026-10-11", bl::RequestStatus::Open));
        state.requests.add(std::make_unique<bl::RoutineRequest>(
            "REQ-103", "ngo_admin", "Graphic Era Health Center",
            bl::BloodGroup::BPos, 5, "2026-10-12", bl::RequestStatus::Open));
        state.requests.save();
    }

    // Seed initial users if empty
    if (state.accounts.all().size() <= 1) {
        std::cout << "[seed] Creating initial users and data...\n";

        state.accounts.add(std::make_unique<bl::Donor>(
            "akshat", "Akshat Sharma", makeCred("123")));
        state.accounts.add(std::make_unique<bl::Donor>(
            "samar", "Samar Ali Ansari", makeCred("123")));
        state.accounts.add(std::make_unique<bl::Donor>(
            "priya", "Priya Mehra", makeCred("123")));
        state.accounts.add(std::make_unique<bl::Donor>(
            "vikram", "Vikram Joshi", makeCred("123")));
        state.accounts.add(std::make_unique<bl::Donor>(
            "ananya", "Ananya Bisht", makeCred("123")));
        state.accounts.add(std::make_unique<bl::Donor>(
            "kalyani", "kalyani rawat", makeCred("123")));
        state.accounts.add(std::make_unique<bl::NGO>(
            "ngo_admin", "RamSay Blood Bank & Hospital", makeCred("123"), "Approved", "Nainital", "9876543210", "ramsay@ngo.org"));

        state.accounts.save();

        // Seed donor profiles with dates
        {
            bl::Donor d("akshat", "Akshat Sharma", bl::Credential{});
            d.setAge(24); d.setGender("Male");
            d.setBloodGroup(bl::BloodGroup::ONeg);
            d.setWeight(65); d.setPhone("9876543210");
            d.grantConsent(); d.markProfileComplete();
            d.setLastDonationDate("2026-10-06");
            d.setDonationCount(1);
            state.donors.add(std::move(d));
        }
        {
            bl::Donor d("samar", "Samar Ali Ansari", bl::Credential{});
            d.setAge(29); d.setGender("Male");
            d.setBloodGroup(bl::BloodGroup::APos);
            d.setWeight(72); d.setPhone("8765432109");
            d.grantConsent(); d.markProfileComplete();
            d.setLastDonationDate("2026-10-08");
            d.setDonationCount(1);
            state.donors.add(std::move(d));
        }
        {
            bl::Donor d("priya", "Priya Mehra", bl::Credential{});
            d.setAge(26); d.setGender("Female");
            d.setBloodGroup(bl::BloodGroup::BPos);
            d.setWeight(58); d.setPhone("9123456780");
            d.grantConsent(); d.markProfileComplete();
            d.setLastDonationDate("2026-10-10");
            d.setDonationCount(1);
            state.donors.add(std::move(d));
        }
        {
            bl::Donor d("vikram", "Vikram Joshi", bl::Credential{});
            d.setAge(32); d.setGender("Male");
            d.setBloodGroup(bl::BloodGroup::OPos);
            d.setWeight(78); d.setPhone("9234567891");
            d.grantConsent(); d.markProfileComplete();
            d.setLastDonationDate("2026-10-11");
            d.setDonationCount(1);
            state.donors.add(std::move(d));
        }
        {
            bl::Donor d("ananya", "Ananya Bisht", bl::Credential{});
            d.setAge(23); d.setGender("Female");
            d.setBloodGroup(bl::BloodGroup::ANeg);
            d.setWeight(54); d.setPhone("9345678902");
            d.grantConsent(); d.markProfileComplete();
            d.setLastDonationDate("2026-10-05");
            d.setDonationCount(1);
            state.donors.add(std::move(d));
        }
        {
            bl::Donor d("kalyani", "kalyani rawat", bl::Credential{});
            d.setAge(31); d.setGender("Female");
            d.setBloodGroup(bl::BloodGroup::ABPos);
            d.setWeight(58); d.setPhone("7654321098");
            d.revokeConsent(); d.markProfileComplete();
            d.setLastDonationDate("2026-08-15");
            d.setDonationCount(1);
            state.donors.add(std::move(d));
        }
        state.donors.save();

        // Seed camps with active and upcoming October/November 2026 dates
        bl::Camp c1("CAMP-001", "RamSay Hospital, Nainital", "2026-10-18", 80, 160, bl::CampStatus::Open, 12, 1);
        c1.addRegisteredDonorDirect("akshat");
        state.camps.add(std::move(c1));

        bl::Camp c2("CAMP-002", "Graphic Era University Campus", "2026-10-25", 150, 300, bl::CampStatus::Upcoming, 0, 0);
        c2.addRegisteredDonorDirect("samar");
        state.camps.add(std::move(c2));

        state.camps.add(bl::Camp("CAMP-003", "Central Himalayan Environment Association (CHEA)",
                                 "2026-11-05", 60, 120, bl::CampStatus::Upcoming, 0, 0));

        state.camps.add(bl::Camp("CAMP-004", "District Red Cross Society, Almora",
                                 "2026-08-15", 50, 100, bl::CampStatus::Completed, 50, 3));
        state.camps.save();

        // Seed donations: fresh viable units (October 2026) and expired units (August 2026)
        state.donations.add(bl::Donation("R-2201", "akshat", "Akshat Sharma", "O-",
                                         "CAMP-001", "RamSay Hospital, Nainital", "2026-10-06", "Completed"));
        state.donations.add(bl::Donation("R-2202", "samar", "Samar Ali Ansari", "A+",
                                         "CAMP-001", "RamSay Hospital, Nainital", "2026-10-08", "Completed"));
        state.donations.add(bl::Donation("R-2203", "priya", "Priya Mehra", "B+",
                                         "CAMP-001", "RamSay Hospital, Nainital", "2026-10-10", "Completed"));
        state.donations.add(bl::Donation("R-2204", "vikram", "Vikram Joshi", "O+",
                                         "CAMP-001", "RamSay Hospital, Nainital", "2026-10-11", "Completed"));
        state.donations.add(bl::Donation("R-2205", "ananya", "Ananya Bisht", "A-",
                                         "CAMP-001", "RamSay Hospital, Nainital", "2026-10-05", "Completed"));
        state.donations.add(bl::Donation("R-2101", "kalyani", "kalyani rawat", "AB+",
                                         "CAMP-004", "District Red Cross Society, Almora", "2026-08-15", "Completed"));
        state.donations.add(bl::Donation("R-2102", "samar", "Samar Ali Ansari", "A+",
                                         "CAMP-004", "District Red Cross Society, Almora", "2026-08-12", "Completed"));
        state.donations.save();

        state.audit.log("system", "SEED_DATA", "Initialized default accounts and camps");
        std::cout << "[seed] Default data initialized successfully.\n";
    }
}

// ───────────────────────────────────────────────
// Auth and Security helpers
// ───────────────────────────────────────────────

void validateStrings(const json& value) {
    if (value.is_string()) {
        const auto& text = value.get_ref<const std::string&>();
        if (text.size() > 1000 || text.find_first_of("\r\n") != std::string::npos) throw bl::BadRequestError("Text fields must be a single line with at most 1000 characters.");
    } else if (value.is_structured()) for (const auto& item : value) validateStrings(item);
}

void checkPayloadSize(const httplib::Request& req) {
    if (!req.body.empty()) validateStrings(json::parse(req.body));
    if (req.body.size() > 65536) { // 64 KB limit
        throw bl::BadRequestError("Payload exceeds maximum size of 64KB");
    }
}

std::string extractToken(const httplib::Request& req) {
    auto auth = req.get_header_value("Authorization");
    const std::string prefix = "Bearer ";
    if (auth.size() > prefix.size() && auth.substr(0, prefix.size()) == prefix) {
        return auth.substr(prefix.size());
    }
    if (req.has_param("token")) {
        return req.get_param_value("token");
    }
    return "";
}

bl::Account* authenticate(const httplib::Request& req, AppState& state) {
    auto token = extractToken(req);
    if (token.empty()) throw bl::UnauthorizedError("No session token provided");

    auto username = state.sessions.validate(token);
    if (username.empty()) throw bl::UnauthorizedError("Session expired or invalid. Please login again.");

    auto* account = state.accounts.findByUsername(username);
    if (!account) throw bl::UnauthorizedError("Account not found");
    return account;
}

bl::Account* requireRole(const httplib::Request& req, AppState& state, const std::string& role) {
    auto* account = authenticate(req, state);
    if (account->role() != role && account->role() != "admin") {
        throw bl::ForbiddenError("Requires " + role + " role");
    }
    if (role == "ngo") {
        auto* org = dynamic_cast<bl::Organization*>(account);
        if (org && !org->isApproved()) throw bl::ForbiddenError("Organization approval is required. Contact the administrator.");
    }
    return account;
}

bl::Account* requireAdmin(const httplib::Request& req, AppState& state) {
    auto* account = authenticate(req, state);
    if (account->role() != "admin") {
        throw bl::ForbiddenError("Requires administrator privileges");
    }
    return account;
}

bool donorHasConsent(const AppState& state, const std::string& username) {
    auto donorRef = state.donors.find(username);
    return donorRef && donorRef->get().hasConsent();
}

void handleError(httplib::Response& res, const bl::AppError& err) {
    res.status = err.statusCode();
    res.set_content(json{{"error", err.what()}}.dump(), "application/json");
}

void handleError(httplib::Response& res, const std::exception& err) {
    const bool invalid = dynamic_cast<const json::exception*>(&err) != nullptr;
    res.status = invalid ? 400 : 500;
    res.set_content(json{{"error", invalid ? "Invalid request: check field types and required values." : "Unable to complete the request. See the server log."}}.dump(),
                    "application/json");
    std::cerr << "[error] " << err.what() << "\n";
}

// ───────────────────────────────────────────────
// REST API Route Definitions
// ───────────────────────────────────────────────

json campForViewer(const bl::Camp& camp, const httplib::Request& req, AppState& state) {
    auto result = camp.toJson();
    const auto username = state.sessions.validate(extractToken(req));
    const auto* viewer = state.accounts.findByUsername(username);
    const auto* org = dynamic_cast<const bl::Organization*>(viewer);
    const bool manager = viewer && (viewer->role() == "admin" || (org && org->isApproved()));
    const bool booked = std::find(camp.registeredDonors().begin(), camp.registeredDonors().end(), username) != camp.registeredDonors().end();
    result["isRegistered"] = booked;
    if (!manager) result["registeredDonors"] = booked ? json::array({username}) : json::array();
    return result;
}

void validateDate(const std::string& value) {
    if (!bl::DateUtil::isValid(value)) throw bl::BadRequestError("Enter a valid date in YYYY-MM-DD format.");
}

void setupRoutes(httplib::Server& server, AppState& state) {
    server.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(json{{"status", "ok"}, {"service", "bloodline"}}.dump(), "application/json");
    });

    // ── POST /api/auth/register ──
    server.Post("/api/auth/register", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            checkPayloadSize(req);
            auto body = json::parse(req.body);
            auto fullName = body.at("fullName").get<std::string>();
            auto username = body.at("username").get<std::string>();
            auto password = body.at("password").get<std::string>();
            auto role = body.at("role").get<std::string>();

            if (role != "donor" && role != "ngo") throw bl::BadRequestError("Choose donor or organization registration.");
            if (!std::regex_match(username, std::regex("[A-Za-z0-9_]{3,30}"))) throw bl::BadRequestError("Username must use 3–30 letters, numbers, or underscores.");
            if (fullName.empty() || username.empty() || password.empty()) {
                throw bl::BadRequestError("All fields are required");
            }
            if (username.length() < 3 || username.length() > 30) {
                throw bl::BadRequestError("Username must be between 3 and 30 characters");
            }
            if (password.length() < 3) {
                throw bl::BadRequestError("Password must be at least 3 characters long");
            }

            if (state.accounts.findByUsername(username)) {
                throw bl::ConflictError("Username already exists");
            }

            auto cred = bl::Credential::fromPassword(password);
            std::string approvalStatus = (role == "ngo") ? "Pending" : "Approved";
            std::string city = body.value("city", "Nainital");
            std::string orgType = body.value("orgType", "ngo");

            // Design Pattern: Factory for account creation
            auto newAccount = bl::AccountFactory::createAccount(
                role, username, fullName, std::move(cred), orgType, approvalStatus, city);

            state.accounts.add(std::move(newAccount));
            state.accounts.save();

            state.audit.log(username, "USER_REGISTER", "Registered account with role: " + role);

            auto* account = state.accounts.findByUsername(username);
            auto token = state.sessions.createSession(username);

            res.status = 201;
            res.set_content(json{
                {"token", token},
                {"user", account->toJson()}
            }.dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── POST /api/auth/login ── (Rate-limited: 5 attempts = 5 min lock)
    server.Post("/api/auth/login", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            checkPayloadSize(req);
            auto body = json::parse(req.body);
            auto username = body.at("username").get<std::string>();
            auto password = body.at("password").get<std::string>();

            if (state.rateLimiter.isLocked(username)) {
                int secs = state.rateLimiter.remainingLockSeconds(username);
                state.audit.log(username, "LOGIN_LOCKED", "Rate limit lockout active");
                throw bl::TooManyRequestsError(
                    "Account locked due to 5 consecutive failed attempts. Try again in " +
                    std::to_string(secs) + " seconds.");
            }

            auto* account = state.accounts.findByUsername(username);
            if (!account || !account->verifyPassword(password)) {
                state.rateLimiter.recordFailure(username);
                state.audit.log(username, "LOGIN_FAILED", "Bad credentials");
                throw bl::UnauthorizedError("Invalid username or password");
            }

            // Success resets rate limiter counter
            state.rateLimiter.recordSuccess(username);
            auto token = state.sessions.createSession(username);

            state.audit.log(username, "LOGIN_SUCCESS", "Logged in successfully");

            res.set_content(json{
                {"token", token},
                {"user", account->toJson()}
            }.dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── POST /api/auth/logout ──
    server.Post("/api/auth/logout", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto token = extractToken(req);
            if (!token.empty()) {
                auto username = state.sessions.validate(token);
                if (!username.empty()) {
                    state.audit.log(username, "LOGOUT", "Session invalidated");
                }
                state.sessions.invalidate(token);
            }
            res.set_content(json{{"message", "Logged out"}}.dump(), "application/json");
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/me ──
    server.Get("/api/me", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto* account = authenticate(req, state);
            res.set_content(account->toJson().dump(), "application/json");
        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── PUT /api/donor/account ── (Edit name, phone, password with current pw)
    server.Put("/api/donor/account", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            checkPayloadSize(req);
            auto* account = authenticate(req, state);
            auto body = json::parse(req.body);

            auto currentPassword = body.at("currentPassword").get<std::string>();
            if (!account->verifyPassword(currentPassword)) {
                throw bl::BadRequestError("Current password verification failed");
            }

            if (body.contains("newPassword") && !body["newPassword"].get<std::string>().empty() && body["newPassword"].get<std::string>().length() < 3) throw bl::BadRequestError("New password must be at least 3 characters.");
            if (body.contains("fullName") && !body["fullName"].get<std::string>().empty()) {
                account->setFullName(body["fullName"].get<std::string>());
            }

            if (body.contains("newPassword") && !body["newPassword"].get<std::string>().empty()) {
                std::string newPw = body["newPassword"].get<std::string>();
                if (newPw.length() < 3) throw bl::BadRequestError("New password must be at least 3 chars");
                account->updateCredential(bl::Credential::fromPassword(newPw));
            }

            state.accounts.save();

            // Keep the profile and account display name in sync.
            auto donorRef = state.donors.find(account->username());
            if (donorRef) {
                donorRef->get().setFullName(account->fullName());
                state.donors.save();
            }
            if (donorRef && body.contains("phone") && !body["phone"].get<std::string>().empty()) {
                donorRef->get().setPhone(body["phone"].get<std::string>());
                state.donors.save();
            }

            state.audit.log(account->username(), "PROFILE_UPDATE", "Updated account credentials");
            res.set_content(json{{"message", "Account updated successfully"}}.dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/donor/profile ──
    server.Get("/api/donor/profile", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto* account = requireRole(req, state, "donor");
            auto donorRef = state.donors.find(account->username());
            if (!donorRef) {
                res.set_content("null", "application/json");
                return;
            }
            res.set_content(donorRef->get().profileToJson(false).dump(), "application/json");
        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── PUT /api/donor/profile ──
    server.Put("/api/donor/profile", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            checkPayloadSize(req);
            auto* account = requireRole(req, state, "donor");
            auto body = json::parse(req.body);

            auto donorRef = state.donors.find(account->username());
            if (donorRef) {
                auto donor = donorRef->get();
                donor.setAge(body.at("age").get<int>());
                donor.setGender(body.at("gender").get<std::string>());
                donor.setBloodGroup(bl::bloodGroupFromString(body.at("bloodGroup").get<std::string>()));
                donor.setWeight(body.at("weight").get<double>());
                donor.setPhone(body.at("phone").get<std::string>());
                if (body.at("consent").get<bool>()) donor.grantConsent();
                else donor.revokeConsent();
                donor.markProfileComplete();
                donorRef->get() = std::move(donor);
            } else {
                bl::Donor donor(account->username(), account->fullName(), bl::Credential{});
                donor.setAge(body.at("age").get<int>());
                donor.setGender(body.at("gender").get<std::string>());
                donor.setBloodGroup(bl::bloodGroupFromString(body.at("bloodGroup").get<std::string>()));
                donor.setWeight(body.at("weight").get<double>());
                donor.setPhone(body.at("phone").get<std::string>());
                if (body.at("consent").get<bool>()) donor.grantConsent();
                else donor.revokeConsent();
                donor.markProfileComplete();
                state.donors.add(std::move(donor));
            }

            state.donors.save();
            state.audit.log(account->username(), "DONOR_PROFILE_SAVE", "Saved medical details");

            auto updatedRef = state.donors.find(account->username());
            res.set_content(updatedRef->get().profileToJson(false).dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/donor/eligibility ── (Includes 90/120 day waiting period check)
    server.Get("/api/donor/eligibility", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto* account = requireRole(req, state, "donor");
            auto donorRef = state.donors.find(account->username());
            if (!donorRef) {
                throw bl::BadRequestError("Please fill out your donor profile first");
            }

            const auto& donor = donorRef->get();
            std::optional<double> hb;
            if (req.has_param("hb") && !req.get_param_value("hb").empty()) {
                hb = std::stod(req.get_param_value("hb"));
            }

            auto result = state.eligibility.evaluate(donor.age(), donor.gender(), donor.weight(), hb);

            // Add waiting period check
            bool waitPass = donor.isEligibleOnDate();
            if (!waitPass) {
                result.eligible = false;
                result.checks.push_back({
                    "Waiting Period",
                    "fail",
                    "Last donated on " + donor.lastDonationDate() +
                    ". Next eligible date is " + donor.nextEligibleDate() +
                    " (" + std::to_string(donor.daysUntilEligible()) + " days remaining)"
                });
            } else if (!donor.lastDonationDate().empty()) {
                result.checks.push_back({
                    "Waiting Period",
                    "pass",
                    "Waiting period fulfilled. Eligible to donate today."
                });
            }

            json checks = json::array();
            for (const auto& c : result.checks) {
                checks.push_back({{"label", c.label}, {"status", c.status}, {"message", c.message}});
            }

            res.set_content(json{
                {"eligible", result.eligible},
                {"checks", checks},
                {"nextEligibleDate", donor.nextEligibleDate()},
                {"daysUntilEligible", donor.daysUntilEligible()}
            }.dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/donor/card ── (Donor printable card & milestone badges)
    server.Get("/api/donor/card", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto* account = requireRole(req, state, "donor");
            auto donorRef = state.donors.find(account->username());
            if (!donorRef) throw bl::BadRequestError("Complete donor profile first");

            const auto& donor = donorRef->get();
            res.set_content(json{
                {"fullName", donor.fullName()},
                {"username", donor.username()},
                {"bloodGroup", donor.bloodGroupString()},
                {"donationCount", donor.donationCount()},
                {"lastDonationDate", donor.lastDonationDate()},
                {"nextEligibleDate", donor.nextEligibleDate()},
                {"milestones", donor.milestoneBadges()}
            }.dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/donor/matching-requests ── (Find urgent requests matching donor)
    server.Get("/api/donor/matching-requests", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto* account = requireRole(req, state, "donor");
            auto donorRef = state.donors.find(account->username());
            if (!donorRef || !donorRef->get().bloodGroup()) {
                res.set_content("[]", "application/json");
                return;
            }

            const auto& donor = donorRef->get();
            json matches = json::array();

            for (const auto& requestPtr : state.requests.all()) {
                if (requestPtr->status() != bl::RequestStatus::Open) continue;

                // Compatible if donor can donate to the requested blood group
                if (state.compatibility.canDonateTo(*donor.bloodGroup(), requestPtr->bloodGroup())) {
                    auto j = requestPtr->toJson();
                    j["isEligible"] = donor.isEligibleOnDate() && donor.hasConsent();

                    // Check if this donor already pledged
                    const auto& responders = requestPtr->responders();
                    j["hasPledged"] = (std::find(responders.begin(), responders.end(), account->username()) != responders.end());

                    // Enrich with organization contact info
                    auto* orgAcc = state.accounts.findByUsername(requestPtr->orgUsername());
                    if (orgAcc) {
                        auto* org = dynamic_cast<const bl::Organization*>(orgAcc);
                        if (org) {
                            j["orgPhone"] = !org->contactPhone().empty() ? org->contactPhone() : "05942-235022";
                            j["orgEmail"] = !org->contactEmail().empty() ? org->contactEmail() : "bloodbank@ramsayhospital.org";
                            j["orgCity"] = org->city();
                        }
                    }

                    matches.push_back(j);
                }
            }

            res.set_content(matches.dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/donor/appointments ── (Donor's upcoming booked camp slots)
    server.Get("/api/donor/appointments", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto* account = requireRole(req, state, "donor");
            json appointments = json::array();

            for (const auto& camp : state.camps.all()) {
                const auto& donors = camp.registeredDonors();
                if (std::find(donors.begin(), donors.end(), account->username()) != donors.end()) {
                    appointments.push_back(camp.toJson());
                }
            }

            res.set_content(appointments.dump(), "application/json");
        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/donor/pledged-requests ── (Urgent requests donor committed to)
    server.Get("/api/donor/pledged-requests", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto* account = requireRole(req, state, "donor");
            json pledged = json::array();

            for (const auto& r : state.requests.all()) {
                const auto& responders = r->responders();
                if (std::find(responders.begin(), responders.end(), account->username()) != responders.end()) {
                    auto rJson = r->toJson();
                    auto* orgAcc = state.accounts.findByUsername(r->orgUsername());
                    if (orgAcc) {
                        auto* org = dynamic_cast<const bl::Organization*>(orgAcc);
                        if (org) {
                            rJson["orgPhone"] = !org->contactPhone().empty() ? org->contactPhone() : "05942-235022";
                            rJson["orgEmail"] = !org->contactEmail().empty() ? org->contactEmail() : "bloodbank@ramsayhospital.org";
                            rJson["orgCity"] = org->city();
                        }
                    }
                    pledged.push_back(rJson);
                }
            }

            res.set_content(pledged.dump(), "application/json");
        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/donor/history ──
    server.Get("/api/donor/history", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto* account = requireRole(req, state, "donor");
            json records = json::array();
            for (const auto& d : state.donations.all()) {
                if (d.donorUsername() == account->username()) {
                    records.push_back(d.toJson());
                }
            }
            res.set_content(records.dump(), "application/json");
        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/camps ── (Public camps list)
    server.Get("/api/camps", [&](const httplib::Request& req, httplib::Response& res) {
        json camps = json::array();
        for (const auto& c : state.camps.all()) {
            camps.push_back(campForViewer(c, req, state));
        }
        res.set_content(camps.dump(), "application/json");
    });

    // ── GET /api/camps/:id ── (Camp detail view)
    server.Get(R"(/api/camps/(CAMP-\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            std::string id = req.matches[1];
            auto campRef = state.camps.find(id);
            if (!campRef) throw bl::NotFoundError("Camp not found");
            res.set_content(campForViewer(campRef->get(), req, state).dump(), "application/json");
        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── POST /api/camps ── (Create camp)
    server.Post("/api/camps", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            checkPayloadSize(req);
            auto* account = requireRole(req, state, "ngo");
            auto* org = dynamic_cast<bl::Organization*>(account);
            if (org && !org->isApproved()) {
                throw bl::ForbiddenError("Organization account is pending admin approval.");
            }

            auto body = json::parse(req.body);
            auto name = body.at("name").get<std::string>();
            auto date = body.at("date").get<std::string>();
            int target = body.at("targetUnits").get<int>();
            int slotCap = body.value("slotCapacity", target * 2);

            if (name.empty() || date.empty() || target <= 0) {
                throw bl::BadRequestError("Invalid camp details");
            }

            validateDate(date);
            if (date < bl::DateUtil::today()) throw bl::BadRequestError("Camp date cannot be in the past.");
            if (target > 10000 || slotCap < 1 || slotCap > 20000) throw bl::BadRequestError("Enter realistic positive targets and capacity.");
            std::string campId = "CAMP-" + std::to_string(state.camps.all().size() + 1);
            while (campId.size() < 8) campId.insert(5, "0");

            state.camps.add(bl::Camp(campId, name, date, target, slotCap));
            state.camps.save();

            state.audit.log(account->username(), "CAMP_CREATE", "Created camp " + campId + " (" + name + ")");

            res.status = 201;
            res.set_content(state.camps.all().back().toJson().dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── PUT /api/camps/:id ── (Edit date and target before start)
    server.Put(R"(/api/camps/(CAMP-\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            checkPayloadSize(req);
            auto* account = requireRole(req, state, "ngo");
            std::string id = req.matches[1];
            auto campRef = state.camps.find(id);
            if (!campRef) throw bl::NotFoundError("Camp not found");

            auto body = json::parse(req.body);
            std::string newDate = body.at("date").get<std::string>();
            int newTarget = body.at("targetUnits").get<int>();

            validateDate(newDate);
            if (newDate < bl::DateUtil::today()) throw bl::BadRequestError("Camp date cannot be in the past.");
            campRef->get().editDetails(newDate, newTarget);
            state.camps.save();

            state.audit.log(account->username(), "CAMP_EDIT", "Edited date/target for " + id);
            res.set_content(campRef->get().toJson().dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── PUT /api/camps/:id/status ── (State machine status change)
    server.Put(R"(/api/camps/(CAMP-\d+)/status)", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            checkPayloadSize(req);
            auto* account = requireRole(req, state, "ngo");
            std::string id = req.matches[1];
            auto campRef = state.camps.find(id);
            if (!campRef) throw bl::NotFoundError("Camp not found");

            auto body = json::parse(req.body);
            std::string statusStr = body.at("status").get<std::string>();
            if (statusStr != "Upcoming" && statusStr != "Open" && statusStr != "Full" && statusStr != "Completed" && statusStr != "Cancelled") throw bl::BadRequestError("Invalid camp status.");
            bl::CampStatus newStatus = bl::campStatusFromString(statusStr);

            campRef->get().setStatus(newStatus);
            state.camps.save();

            state.audit.log(account->username(), "CAMP_STATUS", "Changed " + id + " to " + statusStr);
            res.set_content(campRef->get().toJson().dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── POST /api/camps/:id/register ── (Donor registers for camp slot)
    server.Post(R"(/api/camps/(CAMP-\d+)/register)", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto* account = requireRole(req, state, "donor");
            std::string id = req.matches[1];
            auto campRef = state.camps.find(id);
            if (!campRef) throw bl::NotFoundError("Camp not found");

            if (campRef->get().date() < bl::DateUtil::today()) throw bl::BadRequestError("Registration for this camp has ended.");
            auto donorProfile = state.donors.find(account->username());
            if (!donorProfile) throw bl::BadRequestError("Complete your donor profile before reserving a camp.");
            if (!donorProfile->get().isEligibleOnDate(campRef->get().date())) throw bl::BadRequestError("This camp falls within your donation waiting period.");
            campRef->get().registerDonor(account->username());
            state.camps.save();

            state.audit.log(account->username(), "CAMP_REGISTER", "Registered for slot at " + id);
            state.notifications.send(account->username(), "Camp Registration Confirmed",
                                     "You have registered for " + campRef->get().name() + " on " + campRef->get().date());

            res.set_content(json{
                {"message", "Successfully registered for camp"},
                {"camp", campRef->get().toJson()}
            }.dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── DELETE /api/camps/:id/register ── (Donor unregisters / cancels slot)
    server.Delete(R"(/api/camps/(CAMP-\d+)/register)", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto* account = requireRole(req, state, "donor");
            std::string id = req.matches[1];
            auto campRef = state.camps.find(id);
            if (!campRef) throw bl::NotFoundError("Camp not found");

            campRef->get().unregisterDonor(account->username());
            state.camps.save();

            state.audit.log(account->username(), "CAMP_UNREGISTER", "Cancelled slot reservation at " + id);
            state.notifications.send(account->username(), "Camp Reservation Cancelled",
                                     "Your slot at " + campRef->get().name() + " has been cancelled.");

            res.set_content(json{
                {"message", "Successfully cancelled camp reservation"},
                {"camp", campRef->get().toJson()}
            }.dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/org/camps/:id/donors ── (NGO views registered donor roster for a camp)
    server.Get(R"(/api/org/camps/(CAMP-\d+)/donors)", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            requireRole(req, state, "ngo");
            std::string campId = req.matches[1];
            auto campRef = state.camps.find(campId);
            if (!campRef) throw bl::NotFoundError("Camp not found");

            json roster = json::array();
            for (const auto& uname : campRef->get().registeredDonors()) {
                auto dRef = state.donors.find(uname);
                if (dRef) {
                    const auto& d = dRef->get();
                    roster.push_back({
                        {"username", d.username()},
                        {"fullName", d.fullName()},
                        {"bloodGroup", d.bloodGroupString()},
                        {"phone", d.phone()},
                        {"age", d.age()},
                        {"weight", d.weight()},
                        {"isEligibleToday", d.isEligibleOnDate()},
                        {"lastDonationDate", d.lastDonationDate()}
                    });
                } else {
                    auto* acc = state.accounts.findByUsername(uname);
                    roster.push_back({
                        {"username", uname},
                        {"fullName", acc ? acc->fullName() : uname},
                        {"bloodGroup", "Unknown"},
                        {"phone", "N/A"},
                        {"isEligibleToday", true},
                        {"lastDonationDate", ""}
                    });
                }
            }

            res.set_content(json{
                {"campId", campId},
                {"campName", campRef->get().name()},
                {"campDate", campRef->get().date()},
                {"donors", roster}
            }.dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── POST /api/donations ── (Records donation; checks waiting period)
    server.Post("/api/donations", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            checkPayloadSize(req);
            auto* account = requireRole(req, state, "ngo");
            auto* org = dynamic_cast<bl::Organization*>(account);
            if (org && !org->isApproved()) {
                throw bl::ForbiddenError("Organization account is pending approval.");
            }

            auto body = json::parse(req.body);
            auto username = body.at("username").get<std::string>();
            auto campId = body.at("campId").get<std::string>();
            auto date = body.at("date").get<std::string>();
            auto status = body.at("status").get<std::string>();

            if (status != "Completed" && status != "Deferred") {
                throw bl::BadRequestError("Status must be 'Completed' or 'Deferred'");
            }

            auto donorRef = state.donors.find(username);
            if (!donorRef) throw bl::NotFoundError("Donor username not found in database");

            auto campRef = state.camps.find(campId);
            if (!campRef) throw bl::NotFoundError("Camp not found");

            validateDate(date);
            if (date > bl::DateUtil::today()) throw bl::BadRequestError("Cannot record a future donation.");
            if (campRef->get().status() != bl::CampStatus::Open && campRef->get().status() != bl::CampStatus::Full) throw bl::BadRequestError("Start the camp before recording donations.");
            if (date != campRef->get().date()) throw bl::BadRequestError("Donation date must match the camp date.");
            for (const auto& existing : state.donations.all()) {
                if (existing.donorUsername() == username && existing.date() == date && existing.campId() == campId) throw bl::ConflictError("A donation record already exists for this donor at this camp on this date.");
            }
            auto& donor = donorRef->get();

            // Donor safety check: waiting period!
            if (status == "Completed" && !donor.isEligibleOnDate(date)) {
                throw bl::BadRequestError(
                    "Safety Alert: Donor last donated on " + donor.lastDonationDate() +
                    ". Next eligible date is " + donor.nextEligibleDate() + ".");
            }

            std::string donId = "R-" + std::to_string(2200 + state.donations.all().size() + 1);
            auto donation = bl::Donation::record(donId, donor, campId, campRef->get().name(), date, status);

            state.donations.add(std::move(donation));

            if (status == "Completed") {
                campRef->get().addUnit();
                donor.incrementDonationCount(date);
                state.donors.save();
            } else {
                campRef->get().addDeferred();
            }

            state.camps.save();
            state.donations.save();

            state.audit.log(account->username(), "DONATION_RECORD",
                            "Recorded " + status + " donation for " + username + " at " + campId);

            res.status = 201;
            res.set_content(state.donations.all().back().toJson().dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/inventory/stock ── (Org / Admin: blood stock & 42-day expiry)
    server.Get("/api/inventory/stock", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            requireRole(req, state, "ngo");

            std::string today = bl::DateUtil::today();
            std::map<std::string, int> usableUnits;
            std::map<std::string, int> expiredUnits;

            for (const auto& bg : {bl::BloodGroup::APos, bl::BloodGroup::ANeg,
                                   bl::BloodGroup::BPos, bl::BloodGroup::BNeg,
                                   bl::BloodGroup::ABPos, bl::BloodGroup::ABNeg,
                                   bl::BloodGroup::OPos, bl::BloodGroup::ONeg}) {
                usableUnits[bl::bloodGroupToString(bg)] = 0;
                expiredUnits[bl::bloodGroupToString(bg)] = 0;
            }

            json activeUnitList = json::array();
            json expiredUnitList = json::array();

            for (const auto& don : state.donations.all()) {
                if (don.status() != "Completed" && don.status() != "Discarded" && don.status() != "Transfused") continue;
                bl::BloodUnit unit(don.id(), bl::bloodGroupFromString(don.bloodGroup()), don.date());
                auto uJson = unit.toJson(today);
                uJson["campName"] = don.campName();
                uJson["donorName"] = don.donorName();
                uJson["donorUsername"] = don.donorUsername();
                uJson["donationStatus"] = don.status();

                if (don.status() == "Discarded") {
                    uJson["viability"] = "Discarded";
                    expiredUnitList.push_back(uJson);
                } else if (don.status() == "Transfused") {
                    uJson["viability"] = "Transfused";
                    expiredUnitList.push_back(uJson);
                } else if (unit.isExpired(today)) {
                    uJson["viability"] = "Expired";
                    expiredUnits[unit.bloodGroupString()]++;
                    expiredUnitList.push_back(uJson);
                } else {
                    uJson["viability"] = unit.daysRemaining(today) <= 7 ? "Expiring Soon" : "Fresh";
                    usableUnits[unit.bloodGroupString()]++;
                    activeUnitList.push_back(uJson);
                }
            }

            json summary = json::object();
            const int LOW_THRESHOLD = 3;
            for (const auto& [bg, count] : usableUnits) {
                summary[bg] = {
                    {"count", count},
                    {"isLow", count < LOW_THRESHOLD},
                    {"expiredCount", expiredUnits[bg]}
                };
            }

            res.set_content(json{
                {"today", today},
                {"threshold", LOW_THRESHOLD},
                {"stockByGroup", summary},
                {"activeUnits", activeUnitList},
                {"expiredUnits", expiredUnitList}
            }.dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── PUT /api/inventory/units/:id/status ── (Discard expired unit or mark transfused)
    server.Put(R"(/api/inventory/units/([A-Za-z0-9-]+)/status)", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            checkPayloadSize(req);
            auto* account = requireRole(req, state, "ngo");
            std::string unitId = req.matches[1];
            auto donRef = state.donations.find(unitId);
            if (!donRef) throw bl::NotFoundError("Blood unit record not found");

            auto body = json::parse(req.body);
            std::string newStatus = body.at("status").get<std::string>();
            if (newStatus != "Discarded" && newStatus != "Transfused" && newStatus != "Completed") {
                throw bl::BadRequestError("Status must be Discarded, Transfused, or Completed");
            }

            if (newStatus == "Completed" || donRef->get().status() != "Completed") throw bl::BadRequestError("Only available units can be transfused or discarded; disposal is final.");
            if (newStatus == "Transfused" && bl::BloodUnit(donRef->get().id(), bl::bloodGroupFromString(donRef->get().bloodGroup()), donRef->get().date()).isExpired()) throw bl::BadRequestError("Expired units cannot be transfused.");
            donRef->get().setStatus(newStatus);
            state.donations.save();

            state.audit.log(account->username(), "INVENTORY_DISPOSAL", "Updated unit " + unitId + " to " + newStatus);

            res.set_content(json{
                {"message", "Blood unit status updated successfully"},
                {"unitId", unitId},
                {"status", newStatus}
            }.dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/public/stock ── (Public view: Low / Adequate status only)
    server.Get("/api/public/stock", [&](const httplib::Request&, httplib::Response& res) {
        std::string today = bl::DateUtil::today();
        std::map<std::string, int> usableUnits;

        for (const auto& bg : {bl::BloodGroup::APos, bl::BloodGroup::ANeg,
                               bl::BloodGroup::BPos, bl::BloodGroup::BNeg,
                               bl::BloodGroup::ABPos, bl::BloodGroup::ABNeg,
                               bl::BloodGroup::OPos, bl::BloodGroup::ONeg}) {
            usableUnits[bl::bloodGroupToString(bg)] = 0;
        }

        for (const auto& don : state.donations.all()) {
            if (don.status() != "Completed") continue;
            bl::BloodUnit unit(don.id(), bl::bloodGroupFromString(don.bloodGroup()), don.date());
            if (!unit.isExpired(today)) {
                usableUnits[unit.bloodGroupString()]++;
            }
        }

        json publicStock = json::object();
        for (const auto& [bg, count] : usableUnits) {
            publicStock[bg] = {
                {"status", count < 3 ? "Low" : "Adequate"},
                {"count", count}
            };
        }

        res.set_content(publicStock.dump(), "application/json");
    });

    // ── GET /api/requests ── (Lists open blood requests prioritized)
    server.Get("/api/requests", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            std::vector<const bl::Request*> sortedReqs;
            for (const auto& r : state.requests.all()) {
                sortedReqs.push_back(r.get());
            }

            // Polymorphic sorting using RequestPrioritizer
            bl::RequestPrioritizer::sort(sortedReqs);

            json list = json::array();
            for (const auto* r : sortedReqs) {
                list.push_back(r->toJson());
            }

            res.set_content(list.dump(), "application/json");

        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── POST /api/requests ── (Orgs create Urgent or Routine requests)
    server.Post("/api/requests", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            checkPayloadSize(req);
            auto* account = requireRole(req, state, "ngo");
            auto body = json::parse(req.body);

            auto bgStr = body.at("bloodGroup").get<std::string>();
            int units = body.at("unitsNeeded").get<int>();
            std::string hospital = body.value("hospitalName", account->fullName());
            std::string urgency = body.value("urgency", "Normal");

            if (units <= 0) throw bl::BadRequestError("Units needed must be positive");

            std::string reqId = "REQ-" + std::to_string(100 + state.requests.all().size() + 1);
            bl::BloodGroup bg = bl::bloodGroupFromString(bgStr);

            std::unique_ptr<bl::Request> newReq;
            if (urgency == "Critical") {
                newReq = std::make_unique<bl::UrgentRequest>(reqId, account->username(), hospital, bg, units, true);
            } else if (urgency == "Urgent") {
                newReq = std::make_unique<bl::UrgentRequest>(reqId, account->username(), hospital, bg, units, false);
            } else {
                newReq = std::make_unique<bl::RoutineRequest>(reqId, account->username(), hospital, bg, units);
            }

            // Design Pattern: Strategy Matching & Notification
            auto matchedDonors = state.matchingEngine.findMatches(state.donors.all(), *newReq);
            for (const auto* d : matchedDonors) {
                state.notifications.send(
                    d->username(), "Urgent Blood Need Alert",
                    "BloodLine: " + hospital + " has an urgent request for " + bgStr + " blood.");
            }

            state.requests.add(std::move(newReq));
            state.requests.save();

            state.audit.log(account->username(), "REQUEST_CREATE",
                            "Created " + urgency + " request " + reqId + " for " + bgStr);

            res.status = 201;
            res.set_content(state.requests.all().back()->toJson().dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── POST /api/requests/:id/respond ── (Donor pledges to donate)
    server.Post(R"(/api/requests/(REQ-\d+)/respond)", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto* account = requireRole(req, state, "donor");
            std::string reqId = req.matches[1];
            auto* r = state.requests.find(reqId);
            if (!r) throw bl::NotFoundError("Blood request not found");

            auto matches = state.matchingEngine.findMatches(state.donors.all(), *r);
            if (std::none_of(matches.begin(), matches.end(), [&](const auto* donor) { return donor->username() == account->username(); })) throw bl::BadRequestError("Your donor profile must be eligible, compatible, and consented to pledge.");
            r->addResponder(account->username());
            state.requests.save();

            state.audit.log(account->username(), "REQUEST_PLEDGE", "Pledged to donate for " + reqId);
            state.notifications.send(r->orgUsername(), "Donor Response Received",
                                     "Donor @" + account->username() + " pledged to donate for " + reqId);

            res.set_content(r->toJson().dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── PUT /api/requests/:id/status ── (Fulfill or close request)
    server.Put(R"(/api/requests/(REQ-\d+)/status)", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            checkPayloadSize(req);
            auto* account = requireRole(req, state, "ngo");
            std::string reqId = req.matches[1];
            auto* r = state.requests.find(reqId);
            if (!r) throw bl::NotFoundError("Request not found");

            auto body = json::parse(req.body);
            std::string st = body.at("status").get<std::string>();
            if (st == "Fulfilled") r->markFulfilled();
            else if (st == "Closed") r->markClosed();
            else throw bl::BadRequestError("Status must be Fulfilled or Closed");

            state.requests.save();
            state.audit.log(account->username(), "REQUEST_STATUS", "Updated " + reqId + " to " + st);

            res.set_content(r->toJson().dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/org/requests/:id/responders ── (NGO views donor responders list with phone numbers)
    server.Get(R"(/api/org/requests/(REQ-\d+)/responders)", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            requireRole(req, state, "ngo");
            std::string reqId = req.matches[1];
            auto* r = state.requests.find(reqId);
            if (!r) throw bl::NotFoundError("Request not found");

            json respondersList = json::array();
            for (const auto& uname : r->responders()) {
                auto dRef = state.donors.find(uname);
                if (dRef) {
                    const auto& d = dRef->get();
                    respondersList.push_back({
                        {"username", d.username()},
                        {"fullName", d.fullName()},
                        {"bloodGroup", d.bloodGroupString()},
                        {"phone", d.phone()}
                    });
                } else {
                    auto* acc = state.accounts.findByUsername(uname);
                    respondersList.push_back({
                        {"username", uname},
                        {"fullName", acc ? acc->fullName() : uname},
                        {"bloodGroup", "Unknown"},
                        {"phone", "N/A"}
                    });
                }
            }

            res.set_content(json{
                {"requestId", reqId},
                {"bloodGroup", r->bloodGroupString()},
                {"unitsNeeded", r->unitsNeeded()},
                {"responders", respondersList}
            }.dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/org/reminders ── (Donors eligible in next 14 days)
    server.Get("/api/org/reminders", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            requireRole(req, state, "ngo");
            std::string today = bl::DateUtil::today();
            std::string in14Days = bl::DateUtil::addDays(today, 14);

            std::map<std::string, json> grouped;
            for (const auto& bg : {bl::BloodGroup::APos, bl::BloodGroup::ANeg,
                                   bl::BloodGroup::BPos, bl::BloodGroup::BNeg,
                                   bl::BloodGroup::ABPos, bl::BloodGroup::ABNeg,
                                   bl::BloodGroup::OPos, bl::BloodGroup::ONeg}) {
                grouped[bl::bloodGroupToString(bg)] = json::array();
            }

            for (const auto& d : state.donors.all()) {
                // Privacy filter: consented donors only!
                if (!d.hasConsent() || !d.bloodGroup()) continue;

                std::string nextEl = d.nextEligibleDate();
                if (nextEl.empty() || (nextEl >= today && nextEl <= in14Days)) {
                    grouped[d.bloodGroupString()].push_back({
                        {"username", d.username()},
                        {"fullName", d.fullName()},
                        {"bloodGroup", d.bloodGroupString()},
                        {"phone", d.maskedPhone()}, // Masked phone for privacy
                        {"nextEligibleDate", nextEl.empty() ? "Eligible Now" : nextEl},
                        {"daysRemaining", d.daysUntilEligible(today)}
                    });
                }
            }

            res.set_content(json(grouped).dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/org/donors ── (Consented donors search with filters & pagination)
    server.Get("/api/org/donors", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            requireRole(req, state, "ngo");

            std::string query = req.has_param("q") ? req.get_param_value("q") : "";
            std::string bgFilter = req.has_param("bg") ? req.get_param_value("bg") : "";
            bool eligibleOnly = req.has_param("eligibleOnly") && req.get_param_value("eligibleOnly") == "true";
            int page = req.has_param("page") ? std::stoi(req.get_param_value("page")) : 1;

            std::transform(query.begin(), query.end(), query.begin(), ::tolower);

            std::vector<json> filtered;
            for (const auto& d : state.donors.all()) {
                // Privacy filter: only consented donors appear to organizations!
                if (!d.hasConsent()) continue;

                if (!bgFilter.empty() && d.bloodGroupString() != bgFilter) continue;
                if (eligibleOnly && !d.isEligibleOnDate()) continue;

                if (!query.empty()) {
                    std::string name = d.fullName();
                    std::string bg = d.bloodGroupString();
                    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
                    std::transform(bg.begin(), bg.end(), bg.begin(), ::tolower);
                    if (name.find(query) == std::string::npos && bg.find(query) == std::string::npos) {
                        continue;
                    }
                }
                filtered.push_back(d.profileToJson(true)); // Phone is masked!
            }

            auto paginated = bl::paginate(filtered, page, 10);
            res.set_content(paginated.toJson([](const json& item) { return item; }).dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/org/profile & PUT /api/org/profile ──
    server.Get("/api/org/profile", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto* account = requireRole(req, state, "ngo");
            res.set_content(account->toJson().dump(), "application/json");
        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    server.Put("/api/org/profile", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            checkPayloadSize(req);
            auto* account = requireRole(req, state, "ngo");
            auto* org = dynamic_cast<bl::Organization*>(account);
            if (!org) throw bl::BadRequestError("Not an organization account");

            auto body = json::parse(req.body);
            if (body.contains("fullName")) org->setFullName(body["fullName"].get<std::string>());
            if (body.contains("city")) org->setCity(body["city"].get<std::string>());
            if (body.contains("contactPhone")) org->setContactPhone(body["contactPhone"].get<std::string>());
            if (body.contains("contactEmail")) org->setContactEmail(body["contactEmail"].get<std::string>());

            state.accounts.save();
            state.audit.log(account->username(), "ORG_PROFILE_UPDATE", "Updated organization info");

            res.set_content(org->toJson().dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── CSV Export 1: /api/org/export/donors.csv ── (Consented donors only, phones masked)
    server.Get("/api/org/export/donors.csv", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            requireRole(req, state, "ngo");
            std::stringstream ss;
            ss << "Username,Full Name,Blood Group,Age,Weight,Phone,Last Donation,Next Eligible Date\n";
            for (const auto& d : state.donors.all()) {
                if (!d.hasConsent()) continue; // Consent filter
                ss << bl::csvField(d.username()) << ","
                   << bl::csvField(d.fullName()) << ","
                   << bl::csvField(d.bloodGroupString()) << ","
                   << d.age() << ","
                   << d.weight() << ","
                   << bl::csvField(d.maskedPhone()) << "," // Security: masked phone
                   << bl::csvField(d.lastDonationDate()) << ","
                   << bl::csvField(d.nextEligibleDate()) << "\n";
            }
            res.set_content(ss.str(), "text/csv");
            res.set_header("Content-Disposition", "attachment; filename=\"consented_donors.csv\"");
        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── CSV Export 2: /api/org/export/camps.csv ──
    server.Get("/api/org/export/camps.csv", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            requireRole(req, state, "ngo");
            std::stringstream ss;
            ss << "Camp ID,Camp Name,Date,Target Units,Units Collected,Deferred Count,Status\n";
            for (const auto& c : state.camps.all()) {
                ss << bl::csvField(c.id()) << ","
                   << bl::csvField(c.name()) << ","
                   << bl::csvField(c.date()) << ","
                   << c.targetUnits() << ","
                   << c.unitsCollected() << ","
                   << c.deferredCount() << ","
                   << bl::csvField(c.statusString()) << "\n";
            }
            res.set_content(ss.str(), "text/csv");
            res.set_header("Content-Disposition", "attachment; filename=\"camp_results.csv\"");
        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/admin/orgs ── (Admin approval board)
    server.Get("/api/admin/orgs", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            requireAdmin(req, state);
            json orgs = json::array();
            for (const auto& acc : state.accounts.all()) {
                if (auto* org = dynamic_cast<bl::Organization*>(acc.get())) {
                    orgs.push_back(org->toJson());
                }
            }
            res.set_content(orgs.dump(), "application/json");
        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── PUT /api/admin/orgs/:username/status ── (Approve/Reject org)
    server.Put(R"(/api/admin/orgs/([^/]+)/status)", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            checkPayloadSize(req);
            auto* admin = requireAdmin(req, state);
            std::string orgUser = req.matches[1];

            auto* acc = state.accounts.findByUsername(orgUser);
            if (!acc) throw bl::NotFoundError("Organization account not found");
            auto* org = dynamic_cast<bl::Organization*>(acc);
            if (!org) throw bl::BadRequestError("Account is not an organization");

            auto body = json::parse(req.body);
            std::string newStatus = body.at("status").get<std::string>();
            if (newStatus != "Approved" && newStatus != "Rejected") {
                throw bl::BadRequestError("Status must be Approved or Rejected");
            }

            org->setApprovalStatus(newStatus);
            state.accounts.save();

            state.audit.log(admin->username(), "ORG_APPROVAL",
                            "Set " + orgUser + " status to " + newStatus);
            state.notifications.send(orgUser, "Account Status Updated",
                                     "Your BloodLine organization account has been " + newStatus);

            res.set_content(org->toJson().dump(), "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/admin/audit-log ── (Admin view with pagination)
    server.Get("/api/admin/audit-log", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            requireAdmin(req, state);
            int page = req.has_param("page") ? std::stoi(req.get_param_value("page")) : 1;
            auto entries = state.audit.all();
            std::reverse(entries.begin(), entries.end()); // newest first

            auto paginated = bl::paginate(entries, page, 10);
            res.set_content(paginated.toJson([](const bl::AuditEntry& e) { return e.toJson(); }).dump(),
                            "application/json");

        } catch (const bl::AppError& e) { handleError(res, e);
        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/public/records ── (Consented donors only; with pagination)
    server.Get("/api/public/records", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            std::string query = req.has_param("q") ? req.get_param_value("q") : "";
            int page = req.has_param("page") ? std::stoi(req.get_param_value("page")) : 1;
            std::transform(query.begin(), query.end(), query.begin(), ::tolower);

            std::vector<json> filtered;
            for (const auto& d : state.donations.all()) {
                // Privacy filter: skip non-consented donors completely
                if (!donorHasConsent(state, d.donorUsername())) continue;

                if (!query.empty()) {
                    std::string name = d.donorName();
                    std::string bg = d.bloodGroup();
                    std::string camp = d.campName();
                    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
                    std::transform(bg.begin(), bg.end(), bg.begin(), ::tolower);
                    std::transform(camp.begin(), camp.end(), camp.begin(), ::tolower);
                    if (name.find(query) == std::string::npos &&
                        bg.find(query) == std::string::npos &&
                        camp.find(query) == std::string::npos) {
                        continue;
                    }
                }
                filtered.push_back(d.toJson());
            }

            auto paginated = bl::paginate(filtered, page, 10);
            res.set_content(paginated.toJson([](const json& item) { return item; }).dump(), "application/json");

        } catch (const std::exception& e) { handleError(res, e); }
    });

    // ── GET /api/compatibility ──
    server.Get("/api/compatibility", [&](const httplib::Request&, httplib::Response& res) {
        res.set_content(state.compatibility.toJson().dump(), "application/json");
    });
}

// ───────────────────────────────────────────────
// Main: Server initialization and file mounting
// ───────────────────────────────────────────────

int main(int argc, char* argv[]) {
    try {
        const std::string dataDir = argc > 1 ? argv[1] : "data";
        const std::string frontendDir = argc > 2 ? argv[2] : "frontend";
        const std::string portText = argc > 3 ? argv[3] : "8080";
        if (!std::regex_match(portText, std::regex("[0-9]{1,5}"))) throw std::runtime_error("Port must be a number from 1 to 65535.");
        const int port = std::stoi(portText);
        if (port < 1 || port > 65535) throw std::runtime_error("Port must be a number from 1 to 65535.");
        if (!std::filesystem::is_regular_file(frontendDir + "/index.html")) throw std::runtime_error("Frontend index.html is missing: " + frontendDir);
        std::filesystem::create_directories(dataDir);
        AppState state(dataDir);
        state.loadAll();
        seedData(state);
        httplib::Server server;
        // Repository references are mutable. Serialize complete requests to protect
        // cross-repository operations, and close idle connections promptly.
        server.new_task_queue = [] { return new httplib::ThreadPool(1); };
        server.set_keep_alive_max_count(1);
        server.set_read_timeout(10, 0);
        server.set_write_timeout(10, 0);
        server.set_payload_max_length(65536);
        if (!server.set_mount_point("/", frontendDir)) throw std::runtime_error("Unable to mount frontend directory.");
        setupRoutes(server, state);
        if (!server.bind_to_port("127.0.0.1", port)) throw std::runtime_error("Cannot bind localhost:" + std::to_string(port) + ". Another application may be using the port.");
        std::cout << "[server] BloodLine ready at http://localhost:" << port << std::endl;
        if (!server.listen_after_bind()) throw std::runtime_error("HTTP listener stopped unexpectedly.");
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "[startup] " << error.what() << std::endl;
        return 1;
    }
}
