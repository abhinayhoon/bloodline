// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/main.cpp and remains unchanged.
// This brings in "bloodline/Account.hpp", so this file can use its declarations.
#include "bloodline/Account.hpp"
// This brings in "bloodline/AccountFactory.hpp", so this file can use its declarations.
#include "bloodline/AccountFactory.hpp"
// This brings in "bloodline/AuditLogger.hpp", so this file can use its declarations.
#include "bloodline/AuditLogger.hpp"
// This brings in "bloodline/BloodGroup.hpp", so this file can use its declarations.
#include "bloodline/BloodGroup.hpp"
// This brings in "bloodline/BloodUnit.hpp", so this file can use its declarations.
#include "bloodline/BloodUnit.hpp"
// This brings in "bloodline/Camp.hpp", so this file can use its declarations.
#include "bloodline/Camp.hpp"
// This brings in "bloodline/Compatibility.hpp", so this file can use its declarations.
#include "bloodline/Compatibility.hpp"
// This brings in "bloodline/DateUtil.hpp", so this file can use its declarations.
#include "bloodline/DateUtil.hpp"
// This brings in "bloodline/Donation.hpp", so this file can use its declarations.
#include "bloodline/Donation.hpp"
// This brings in "bloodline/Donor.hpp", so this file can use its declarations.
#include "bloodline/Donor.hpp"
// This brings in "bloodline/EligibilityRules.hpp", so this file can use its declarations.
#include "bloodline/EligibilityRules.hpp"
// This brings in "bloodline/Errors.hpp", so this file can use its declarations.
#include "bloodline/Errors.hpp"
// This brings in "bloodline/FileRepository.hpp", so this file can use its declarations.
#include "bloodline/FileRepository.hpp"
// This brings in "bloodline/MatchingEngine.hpp", so this file can use its declarations.
#include "bloodline/MatchingEngine.hpp"
// This brings in "bloodline/NotificationService.hpp", so this file can use its declarations.
#include "bloodline/NotificationService.hpp"
// This brings in "bloodline/Organization.hpp", so this file can use its declarations.
#include "bloodline/Organization.hpp"
// This brings in "bloodline/Request.hpp", so this file can use its declarations.
#include "bloodline/Request.hpp"
// This brings in "bloodline/SessionManager.hpp", so this file can use its declarations.
#include "bloodline/SessionManager.hpp"

// This brings in <httplib.h>, so this file can use its declarations.
#include <httplib.h>
// This brings in <nlohmann/json.hpp>, so this file can use its declarations.
#include <nlohmann/json.hpp>

// This brings in <algorithm>, so this file can use its declarations.
#include <algorithm>
// This brings in <filesystem>, so this file can use its declarations.
#include <filesystem>
// This brings in <iostream>, so this file can use its declarations.
#include <iostream>
// This brings in <map>, so this file can use its declarations.
#include <map>
// This brings in <memory>, so this file can use its declarations.
#include <memory>
// This brings in <sstream>, so this file can use its declarations.
#include <sstream>
// This brings in <string>, so this file can use its declarations.
#include <string>

// This gives a convenient name to a type or namespace used below.
using json = nlohmann::json;
// This groups related C++ names together to avoid clashes with other code.
namespace bl = bloodline;

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Application state: repositories, sessions, engine, services
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
struct AppState {
// This performs one step of the current operation, then continues to the next statement.
  bl::AccountFileRepository accounts;
// This performs one step of the current operation, then continues to the next statement.
  bl::DonorFileRepository donors;
// This performs one step of the current operation, then continues to the next statement.
  bl::CampFileRepository camps;
// This performs one step of the current operation, then continues to the next statement.
  bl::DonationFileRepository donations;
// This performs one step of the current operation, then continues to the next statement.
  bl::RequestFileRepository requests;
// This performs one step of the current operation, then continues to the next statement.
  bl::SessionManager sessions;
// This performs one step of the current operation, then continues to the next statement.
  bl::EligibilityEngine eligibility;
// This performs one step of the current operation, then continues to the next statement.
  bl::CompatibilityTable compatibility;
// This performs one step of the current operation, then continues to the next statement.
  bl::AuditLogger audit;
// This performs one step of the current operation, then continues to the next statement.
  bl::LogNotificationService notifications;
// This performs one step of the current operation, then continues to the next statement.
  bl::RateLimiter rateLimiter;
// This performs one step of the current operation, then continues to the next statement.
  bl::DonorMatchingEngine matchingEngine;

// This creates or updates a value that the following logic uses.
  AppState(const std::string &dataDir)
// This is part of the current expression or block; read it together with the nearby lines.
      : accounts(dataDir + "/accounts.csv"), donors(dataDir + "/donors.csv"),
// This is part of the current expression or block; read it together with the nearby lines.
        camps(dataDir + "/camps.csv"), donations(dataDir + "/donations.csv"),
// This is part of the current expression or block; read it together with the nearby lines.
        requests(dataDir + "/requests.csv"), audit(dataDir + "/audit_log.csv"),
// This is part of the current expression or block; read it together with the nearby lines.
        notifications(dataDir + "/notifications.log"), rateLimiter(5, 300) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void loadAll() {
// This performs one step of the current operation, then continues to the next statement.
    accounts.load();
// This performs one step of the current operation, then continues to the next statement.
    donors.load();
// This performs one step of the current operation, then continues to the next statement.
    camps.load();
// This performs one step of the current operation, then continues to the next statement.
    donations.load();
// This performs one step of the current operation, then continues to the next statement.
    requests.load();
// This performs one step of the current operation, then continues to the next statement.
    syncDonorDonationHistory();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void syncDonorDonationHistory() {
// This is a source comment that explains nearby code.
    // Sync donation counts and last donation date from donations repository
// This is a source comment that explains nearby code.
    // into donors
// This creates or updates a value that the following logic uses.
    std::map<std::string, std::pair<int, std::string>> donorStats;
// This controls which part of the program runs next, based on the current condition or result.
    for (const auto &don : donations.all()) {
// This controls which part of the program runs next, based on the current condition or result.
      if (don.status() == "Completed") {
// This creates or updates a value that the following logic uses.
        auto &stat = donorStats[don.donorUsername()];
// This performs one step of the current operation, then continues to the next statement.
        stat.first++;
// This controls which part of the program runs next, based on the current condition or result.
        if (stat.second.empty() || don.date() > stat.second) {
// This performs one step of the current operation, then continues to the next statement.
          stat.second = don.date();
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This controls which part of the program runs next, based on the current condition or result.
    for (auto &d : donors.all()) {
// This creates or updates a value that the following logic uses.
      auto it = donorStats.find(d.username());
// This controls which part of the program runs next, based on the current condition or result.
      if (it != donorStats.end()) {
// This performs one step of the current operation, then continues to the next statement.
        d.setDonationCount(it->second.first);
// This performs one step of the current operation, then continues to the next statement.
        d.setLastDonationDate(it->second.second);
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This performs one step of the current operation, then continues to the next statement.
    donors.save();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Seed data: initial accounts, camps, and records
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
void seedData(AppState &state) {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  auto makeCred = [](const std::string &pw) {
// This sends a result back to the code that called this function.
    return bl::Credential::fromPassword(pw);
// This closes the block opened above, such as a function, class, or control-flow branch.
  };

// This is a source comment that explains nearby code.
  // Seed Admin account
// This controls which part of the program runs next, based on the current condition or result.
  if (!state.accounts.findByUsername("admin")) {
// This performs one step of the current operation, then continues to the next statement.
    std::cout << "[seed] Creating default admin account...\n";
// This is part of the current expression or block; read it together with the nearby lines.
    state.accounts.add(std::make_unique<bl::Admin>(
// This performs one step of the current operation, then continues to the next statement.
        "admin", "BloodLine Administrator", makeCred("admin123")));
// This performs one step of the current operation, then continues to the next statement.
    state.accounts.save();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Seed requests if empty
// This controls which part of the program runs next, based on the current condition or result.
  if (state.requests.all().empty()) {
// This performs one step of the current operation, then continues to the next statement.
    std::cout << "[seed] Creating default blood requests...\n";
// This is part of the current expression or block; read it together with the nearby lines.
    state.requests.add(std::make_unique<bl::UrgentRequest>(
// This is part of the current expression or block; read it together with the nearby lines.
        "REQ-101", "ngo_admin", "RamSay Hospital, Nainital",
// This performs one step of the current operation, then continues to the next statement.
        bl::BloodGroup::ONeg, 3, true, "2026-10-10", bl::RequestStatus::Open));
// This is part of the current expression or block; read it together with the nearby lines.
    state.requests.add(std::make_unique<bl::UrgentRequest>(
// This is part of the current expression or block; read it together with the nearby lines.
        "REQ-102", "ngo_admin", "BD Pandey District Hospital",
// This performs one step of the current operation, then continues to the next statement.
        bl::BloodGroup::APos, 2, false, "2026-10-11", bl::RequestStatus::Open));
// This is part of the current expression or block; read it together with the nearby lines.
    state.requests.add(std::make_unique<bl::RoutineRequest>(
// This is part of the current expression or block; read it together with the nearby lines.
        "REQ-103", "ngo_admin", "Graphic Era Health Center",
// This performs one step of the current operation, then continues to the next statement.
        bl::BloodGroup::BPos, 5, "2026-10-12", bl::RequestStatus::Open));
// This performs one step of the current operation, then continues to the next statement.
    state.requests.save();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Seed initial users if empty
// This controls which part of the program runs next, based on the current condition or result.
  if (state.accounts.all().size() <= 1) {
// This performs one step of the current operation, then continues to the next statement.
    std::cout << "[seed] Creating initial users and data...\n";

// This is part of the current expression or block; read it together with the nearby lines.
    state.accounts.add(std::make_unique<bl::Donor>("akshat", "Akshat Sharma",
// This performs one step of the current operation, then continues to the next statement.
                                                   makeCred("123")));
// This is part of the current expression or block; read it together with the nearby lines.
    state.accounts.add(std::make_unique<bl::Donor>("samar", "Samar Ali Ansari",
// This performs one step of the current operation, then continues to the next statement.
                                                   makeCred("123")));
// This is part of the current expression or block; read it together with the nearby lines.
    state.accounts.add(
// This performs one step of the current operation, then continues to the next statement.
        std::make_unique<bl::Donor>("priya", "Priya Mehra", makeCred("123")));
// This is part of the current expression or block; read it together with the nearby lines.
    state.accounts.add(
// This performs one step of the current operation, then continues to the next statement.
        std::make_unique<bl::Donor>("vikram", "Vikram Joshi", makeCred("123")));
// This is part of the current expression or block; read it together with the nearby lines.
    state.accounts.add(
// This performs one step of the current operation, then continues to the next statement.
        std::make_unique<bl::Donor>("ananya", "Ananya Bisht", makeCred("123")));
// This is part of the current expression or block; read it together with the nearby lines.
    state.accounts.add(std::make_unique<bl::Donor>("kalyani", "kalyani rawat",
// This performs one step of the current operation, then continues to the next statement.
                                                   makeCred("123")));
// This is part of the current expression or block; read it together with the nearby lines.
    state.accounts.add(std::make_unique<bl::NGO>(
// This is part of the current expression or block; read it together with the nearby lines.
        "ngo_admin", "RamSay Blood Bank & Hospital", makeCred("123"),
// This performs one step of the current operation, then continues to the next statement.
        "Approved", "Nainital", "9876543210", "ramsay@ngo.org"));

// This performs one step of the current operation, then continues to the next statement.
    state.accounts.save();

// This is a source comment that explains nearby code.
    // Seed donor profiles with dates
// This opens a scope for the related statements that follow.
    {
// This performs one step of the current operation, then continues to the next statement.
      bl::Donor d("akshat", "Akshat Sharma", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
      d.setAge(24);
// This performs one step of the current operation, then continues to the next statement.
      d.setGender("Male");
// This performs one step of the current operation, then continues to the next statement.
      d.setBloodGroup(bl::BloodGroup::ONeg);
// This performs one step of the current operation, then continues to the next statement.
      d.setWeight(65);
// This performs one step of the current operation, then continues to the next statement.
      d.setPhone("9876543210");
// This performs one step of the current operation, then continues to the next statement.
      d.grantConsent();
// This performs one step of the current operation, then continues to the next statement.
      d.markProfileComplete();
// This performs one step of the current operation, then continues to the next statement.
      d.setLastDonationDate("2026-10-06");
// This performs one step of the current operation, then continues to the next statement.
      d.setDonationCount(1);
// This performs one step of the current operation, then continues to the next statement.
      state.donors.add(std::move(d));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This opens a scope for the related statements that follow.
    {
// This performs one step of the current operation, then continues to the next statement.
      bl::Donor d("samar", "Samar Ali Ansari", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
      d.setAge(29);
// This performs one step of the current operation, then continues to the next statement.
      d.setGender("Male");
// This performs one step of the current operation, then continues to the next statement.
      d.setBloodGroup(bl::BloodGroup::APos);
// This performs one step of the current operation, then continues to the next statement.
      d.setWeight(72);
// This performs one step of the current operation, then continues to the next statement.
      d.setPhone("8765432109");
// This performs one step of the current operation, then continues to the next statement.
      d.grantConsent();
// This performs one step of the current operation, then continues to the next statement.
      d.markProfileComplete();
// This performs one step of the current operation, then continues to the next statement.
      d.setLastDonationDate("2026-10-08");
// This performs one step of the current operation, then continues to the next statement.
      d.setDonationCount(1);
// This performs one step of the current operation, then continues to the next statement.
      state.donors.add(std::move(d));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This opens a scope for the related statements that follow.
    {
// This performs one step of the current operation, then continues to the next statement.
      bl::Donor d("priya", "Priya Mehra", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
      d.setAge(26);
// This performs one step of the current operation, then continues to the next statement.
      d.setGender("Female");
// This performs one step of the current operation, then continues to the next statement.
      d.setBloodGroup(bl::BloodGroup::BPos);
// This performs one step of the current operation, then continues to the next statement.
      d.setWeight(58);
// This performs one step of the current operation, then continues to the next statement.
      d.setPhone("9123456780");
// This performs one step of the current operation, then continues to the next statement.
      d.grantConsent();
// This performs one step of the current operation, then continues to the next statement.
      d.markProfileComplete();
// This performs one step of the current operation, then continues to the next statement.
      d.setLastDonationDate("2026-10-10");
// This performs one step of the current operation, then continues to the next statement.
      d.setDonationCount(1);
// This performs one step of the current operation, then continues to the next statement.
      state.donors.add(std::move(d));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This opens a scope for the related statements that follow.
    {
// This performs one step of the current operation, then continues to the next statement.
      bl::Donor d("vikram", "Vikram Joshi", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
      d.setAge(32);
// This performs one step of the current operation, then continues to the next statement.
      d.setGender("Male");
// This performs one step of the current operation, then continues to the next statement.
      d.setBloodGroup(bl::BloodGroup::OPos);
// This performs one step of the current operation, then continues to the next statement.
      d.setWeight(78);
// This performs one step of the current operation, then continues to the next statement.
      d.setPhone("9234567891");
// This performs one step of the current operation, then continues to the next statement.
      d.grantConsent();
// This performs one step of the current operation, then continues to the next statement.
      d.markProfileComplete();
// This performs one step of the current operation, then continues to the next statement.
      d.setLastDonationDate("2026-10-11");
// This performs one step of the current operation, then continues to the next statement.
      d.setDonationCount(1);
// This performs one step of the current operation, then continues to the next statement.
      state.donors.add(std::move(d));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This opens a scope for the related statements that follow.
    {
// This performs one step of the current operation, then continues to the next statement.
      bl::Donor d("ananya", "Ananya Bisht", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
      d.setAge(23);
// This performs one step of the current operation, then continues to the next statement.
      d.setGender("Female");
// This performs one step of the current operation, then continues to the next statement.
      d.setBloodGroup(bl::BloodGroup::ANeg);
// This performs one step of the current operation, then continues to the next statement.
      d.setWeight(54);
// This performs one step of the current operation, then continues to the next statement.
      d.setPhone("9345678902");
// This performs one step of the current operation, then continues to the next statement.
      d.grantConsent();
// This performs one step of the current operation, then continues to the next statement.
      d.markProfileComplete();
// This performs one step of the current operation, then continues to the next statement.
      d.setLastDonationDate("2026-10-05");
// This performs one step of the current operation, then continues to the next statement.
      d.setDonationCount(1);
// This performs one step of the current operation, then continues to the next statement.
      state.donors.add(std::move(d));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This opens a scope for the related statements that follow.
    {
// This performs one step of the current operation, then continues to the next statement.
      bl::Donor d("kalyani", "kalyani rawat", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
      d.setAge(31);
// This performs one step of the current operation, then continues to the next statement.
      d.setGender("Female");
// This performs one step of the current operation, then continues to the next statement.
      d.setBloodGroup(bl::BloodGroup::ABPos);
// This performs one step of the current operation, then continues to the next statement.
      d.setWeight(58);
// This performs one step of the current operation, then continues to the next statement.
      d.setPhone("7654321098");
// This performs one step of the current operation, then continues to the next statement.
      d.revokeConsent();
// This performs one step of the current operation, then continues to the next statement.
      d.markProfileComplete();
// This performs one step of the current operation, then continues to the next statement.
      d.setLastDonationDate("2026-08-15");
// This performs one step of the current operation, then continues to the next statement.
      d.setDonationCount(1);
// This performs one step of the current operation, then continues to the next statement.
      state.donors.add(std::move(d));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This performs one step of the current operation, then continues to the next statement.
    state.donors.save();

// This is a source comment that explains nearby code.
    // Seed camps with active and upcoming October/November 2026 dates
// This is part of the current expression or block; read it together with the nearby lines.
    bl::Camp c1("CAMP-001", "RamSay Hospital, Nainital", "2026-10-18", 80, 160,
// This performs one step of the current operation, then continues to the next statement.
                bl::CampStatus::Open, 12, 1);
// This performs one step of the current operation, then continues to the next statement.
    c1.addRegisteredDonorDirect("akshat");
// This performs one step of the current operation, then continues to the next statement.
    state.camps.add(std::move(c1));

// This is part of the current expression or block; read it together with the nearby lines.
    bl::Camp c2("CAMP-002", "Graphic Era University Campus", "2026-10-25", 150,
// This performs one step of the current operation, then continues to the next statement.
                300, bl::CampStatus::Upcoming, 0, 0);
// This performs one step of the current operation, then continues to the next statement.
    c2.addRegisteredDonorDirect("samar");
// This performs one step of the current operation, then continues to the next statement.
    state.camps.add(std::move(c2));

// This is part of the current expression or block; read it together with the nearby lines.
    state.camps.add(
// This is part of the current expression or block; read it together with the nearby lines.
        bl::Camp("CAMP-003", "Central Himalayan Environment Association (CHEA)",
// This performs one step of the current operation, then continues to the next statement.
                 "2026-11-05", 60, 120, bl::CampStatus::Upcoming, 0, 0));

// This is part of the current expression or block; read it together with the nearby lines.
    state.camps.add(bl::Camp("CAMP-004", "District Red Cross Society, Almora",
// This is part of the current expression or block; read it together with the nearby lines.
                             "2026-08-15", 50, 100, bl::CampStatus::Completed,
// This performs one step of the current operation, then continues to the next statement.
                             50, 3));
// This performs one step of the current operation, then continues to the next statement.
    state.camps.save();

// This is a source comment that explains nearby code.
    // Seed donations: fresh viable units (October 2026) and expired units
// This is a source comment that explains nearby code.
    // (August 2026)
// This is part of the current expression or block; read it together with the nearby lines.
    state.donations.add(bl::Donation("R-2201", "akshat", "Akshat Sharma", "O-",
// This is part of the current expression or block; read it together with the nearby lines.
                                     "CAMP-001", "RamSay Hospital, Nainital",
// This performs one step of the current operation, then continues to the next statement.
                                     "2026-10-06", "Completed"));
// This is part of the current expression or block; read it together with the nearby lines.
    state.donations.add(
// This is part of the current expression or block; read it together with the nearby lines.
        bl::Donation("R-2202", "samar", "Samar Ali Ansari", "A+", "CAMP-001",
// This performs one step of the current operation, then continues to the next statement.
                     "RamSay Hospital, Nainital", "2026-10-08", "Completed"));
// This is part of the current expression or block; read it together with the nearby lines.
    state.donations.add(bl::Donation("R-2203", "priya", "Priya Mehra", "B+",
// This is part of the current expression or block; read it together with the nearby lines.
                                     "CAMP-001", "RamSay Hospital, Nainital",
// This performs one step of the current operation, then continues to the next statement.
                                     "2026-10-10", "Completed"));
// This is part of the current expression or block; read it together with the nearby lines.
    state.donations.add(bl::Donation("R-2204", "vikram", "Vikram Joshi", "O+",
// This is part of the current expression or block; read it together with the nearby lines.
                                     "CAMP-001", "RamSay Hospital, Nainital",
// This performs one step of the current operation, then continues to the next statement.
                                     "2026-10-11", "Completed"));
// This is part of the current expression or block; read it together with the nearby lines.
    state.donations.add(bl::Donation("R-2205", "ananya", "Ananya Bisht", "A-",
// This is part of the current expression or block; read it together with the nearby lines.
                                     "CAMP-001", "RamSay Hospital, Nainital",
// This performs one step of the current operation, then continues to the next statement.
                                     "2026-10-05", "Completed"));
// This is part of the current expression or block; read it together with the nearby lines.
    state.donations.add(bl::Donation(
// This is part of the current expression or block; read it together with the nearby lines.
        "R-2101", "kalyani", "kalyani rawat", "AB+", "CAMP-004",
// This performs one step of the current operation, then continues to the next statement.
        "District Red Cross Society, Almora", "2026-08-15", "Completed"));
// This is part of the current expression or block; read it together with the nearby lines.
    state.donations.add(bl::Donation(
// This is part of the current expression or block; read it together with the nearby lines.
        "R-2102", "samar", "Samar Ali Ansari", "A+", "CAMP-004",
// This performs one step of the current operation, then continues to the next statement.
        "District Red Cross Society, Almora", "2026-08-12", "Completed"));
// This performs one step of the current operation, then continues to the next statement.
    state.donations.save();

// This is part of the current expression or block; read it together with the nearby lines.
    state.audit.log("system", "SEED_DATA",
// This performs one step of the current operation, then continues to the next statement.
                    "Initialized default accounts and camps");
// This performs one step of the current operation, then continues to the next statement.
    std::cout << "[seed] Default data initialized successfully.\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
  }
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Auth and Security helpers
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
void checkPayloadSize(const httplib::Request &req) {
// This controls which part of the program runs next, based on the current condition or result.
  if (req.body.size() > 65536) { // 64 KB limit
// This reports an error so the caller can handle the failed operation.
    throw bl::BadRequestError("Payload exceeds maximum size of 64KB");
// This closes the block opened above, such as a function, class, or control-flow branch.
  }
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
std::string extractToken(const httplib::Request &req) {
// This creates or updates a value that the following logic uses.
  auto auth = req.get_header_value("Authorization");
// This creates or updates a value that the following logic uses.
  const std::string prefix = "Bearer ";
// This controls which part of the program runs next, based on the current condition or result.
  if (auth.size() > prefix.size() && auth.substr(0, prefix.size()) == prefix) {
// This sends a result back to the code that called this function.
    return auth.substr(prefix.size());
// This closes the block opened above, such as a function, class, or control-flow branch.
  }
// This controls which part of the program runs next, based on the current condition or result.
  if (req.has_param("token")) {
// This sends a result back to the code that called this function.
    return req.get_param_value("token");
// This closes the block opened above, such as a function, class, or control-flow branch.
  }
// This sends a result back to the code that called this function.
  return "";
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
bl::Account *authenticate(const httplib::Request &req, AppState &state) {
// This creates or updates a value that the following logic uses.
  auto token = extractToken(req);
// This controls which part of the program runs next, based on the current condition or result.
  if (token.empty())
// This reports an error so the caller can handle the failed operation.
    throw bl::UnauthorizedError("No session token provided");

// This creates or updates a value that the following logic uses.
  auto username = state.sessions.validate(token);
// This controls which part of the program runs next, based on the current condition or result.
  if (username.empty())
// This reports an error so the caller can handle the failed operation.
    throw bl::UnauthorizedError(
// This performs one step of the current operation, then continues to the next statement.
        "Session expired or invalid. Please login again.");

// This creates or updates a value that the following logic uses.
  auto *account = state.accounts.findByUsername(username);
// This controls which part of the program runs next, based on the current condition or result.
  if (!account)
// This reports an error so the caller can handle the failed operation.
    throw bl::UnauthorizedError("Account not found");
// This sends a result back to the code that called this function.
  return account;
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This creates or updates a value that the following logic uses.
bl::Account *requireRole(const httplib::Request &req, AppState &state,
// This creates or updates a value that the following logic uses.
                         const std::string &role) {
// This creates or updates a value that the following logic uses.
  auto *account = authenticate(req, state);
// This controls which part of the program runs next, based on the current condition or result.
  if (account->role() != role && account->role() != "admin") {
// This reports an error so the caller can handle the failed operation.
    throw bl::ForbiddenError("Requires " + role + " role");
// This closes the block opened above, such as a function, class, or control-flow branch.
  }
// This sends a result back to the code that called this function.
  return account;
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
bl::Account *requireAdmin(const httplib::Request &req, AppState &state) {
// This creates or updates a value that the following logic uses.
  auto *account = authenticate(req, state);
// This controls which part of the program runs next, based on the current condition or result.
  if (account->role() != "admin") {
// This reports an error so the caller can handle the failed operation.
    throw bl::ForbiddenError("Requires administrator privileges");
// This closes the block opened above, such as a function, class, or control-flow branch.
  }
// This sends a result back to the code that called this function.
  return account;
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
bool donorHasConsent(const AppState &state, const std::string &username) {
// This creates or updates a value that the following logic uses.
  auto donorRef = state.donors.find(username);
// This sends a result back to the code that called this function.
  return donorRef && donorRef->get().hasConsent();
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
void handleError(httplib::Response &res, const bl::AppError &err) {
// This performs one step of the current operation, then continues to the next statement.
  res.status = err.statusCode();
// This creates or updates a value that the following logic uses.
  res.set_content(json{{"error", err.what()}}.dump(), "application/json");
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
void handleError(httplib::Response &res, const std::exception &err) {
// This performs one step of the current operation, then continues to the next statement.
  res.status = 500;
// This is part of the current expression or block; read it together with the nearby lines.
  res.set_content(
// This creates or updates a value that the following logic uses.
      json{{"error", "Internal server error: " + std::string(err.what())}}
// This is part of the current expression or block; read it together with the nearby lines.
          .dump(),
// This creates or updates a value that the following logic uses.
      "application/json");
// This performs one step of the current operation, then continues to the next statement.
  std::cerr << "[error] " << err.what() << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// REST API Route Definitions
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
void setupRoutes(httplib::Server &server, AppState &state) {

// This is a source comment that explains nearby code.
  // ── POST /api/auth/register ──
// This creates or updates a value that the following logic uses.
  server.Post("/api/auth/register", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                        httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      checkPayloadSize(req);
// This creates or updates a value that the following logic uses.
      auto body = json::parse(req.body);
// This creates or updates a value that the following logic uses.
      auto fullName = body.at("fullName").get<std::string>();
// This creates or updates a value that the following logic uses.
      auto username = body.at("username").get<std::string>();
// This creates or updates a value that the following logic uses.
      auto password = body.at("password").get<std::string>();
// This creates or updates a value that the following logic uses.
      auto role = body.at("role").get<std::string>();

// This controls which part of the program runs next, based on the current condition or result.
      if (fullName.empty() || username.empty() || password.empty()) {
// This reports an error so the caller can handle the failed operation.
        throw bl::BadRequestError("All fields are required");
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This controls which part of the program runs next, based on the current condition or result.
      if (username.length() < 3 || username.length() > 30) {
// This reports an error so the caller can handle the failed operation.
        throw bl::BadRequestError(
// This performs one step of the current operation, then continues to the next statement.
            "Username must be between 3 and 30 characters");
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This controls which part of the program runs next, based on the current condition or result.
      if (password.length() < 3) {
// This reports an error so the caller can handle the failed operation.
        throw bl::BadRequestError(
// This performs one step of the current operation, then continues to the next statement.
            "Password must be at least 3 characters long");
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This controls which part of the program runs next, based on the current condition or result.
      if (state.accounts.findByUsername(username)) {
// This reports an error so the caller can handle the failed operation.
        throw bl::ConflictError("Username already exists");
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      auto cred = bl::Credential::fromPassword(password);
// This creates or updates a value that the following logic uses.
      std::string approvalStatus = (role == "ngo") ? "Pending" : "Approved";
// This creates or updates a value that the following logic uses.
      std::string city = body.value("city", "Nainital");
// This creates or updates a value that the following logic uses.
      std::string orgType = body.value("orgType", "ngo");

// This is a source comment that explains nearby code.
      // Design Pattern: Factory for account creation
// This creates or updates a value that the following logic uses.
      auto newAccount = bl::AccountFactory::createAccount(
// This is part of the current expression or block; read it together with the nearby lines.
          role, username, fullName, std::move(cred), orgType, approvalStatus,
// This performs one step of the current operation, then continues to the next statement.
          city);

// This performs one step of the current operation, then continues to the next statement.
      state.accounts.add(std::move(newAccount));
// This performs one step of the current operation, then continues to the next statement.
      state.accounts.save();

// This is part of the current expression or block; read it together with the nearby lines.
      state.audit.log(username, "USER_REGISTER",
// This performs one step of the current operation, then continues to the next statement.
                      "Registered account with role: " + role);

// This creates or updates a value that the following logic uses.
      auto *account = state.accounts.findByUsername(username);
// This creates or updates a value that the following logic uses.
      auto token = state.sessions.createSession(username);

// This performs one step of the current operation, then continues to the next statement.
      res.status = 201;
// This is part of the current expression or block; read it together with the nearby lines.
      res.set_content(
// This creates or updates a value that the following logic uses.
          json{{"token", token}, {"user", account->toJson()}}.dump(),
// This creates or updates a value that the following logic uses.
          "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── POST /api/auth/login ── (Rate-limited: 5 attempts = 5 min lock)
// This creates or updates a value that the following logic uses.
  server.Post("/api/auth/login", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                     httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      checkPayloadSize(req);
// This creates or updates a value that the following logic uses.
      auto body = json::parse(req.body);
// This creates or updates a value that the following logic uses.
      auto username = body.at("username").get<std::string>();
// This creates or updates a value that the following logic uses.
      auto password = body.at("password").get<std::string>();

// This controls which part of the program runs next, based on the current condition or result.
      if (state.rateLimiter.isLocked(username)) {
// This creates or updates a value that the following logic uses.
        int secs = state.rateLimiter.remainingLockSeconds(username);
// This performs one step of the current operation, then continues to the next statement.
        state.audit.log(username, "LOGIN_LOCKED", "Rate limit lockout active");
// This reports an error so the caller can handle the failed operation.
        throw bl::TooManyRequestsError("Account locked due to 5 consecutive "
// This is part of the current expression or block; read it together with the nearby lines.
                                       "failed attempts. Try again in " +
// This performs one step of the current operation, then continues to the next statement.
                                       std::to_string(secs) + " seconds.");
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      auto *account = state.accounts.findByUsername(username);
// This controls which part of the program runs next, based on the current condition or result.
      if (!account || !account->verifyPassword(password)) {
// This performs one step of the current operation, then continues to the next statement.
        state.rateLimiter.recordFailure(username);
// This performs one step of the current operation, then continues to the next statement.
        state.audit.log(username, "LOGIN_FAILED", "Bad credentials");
// This reports an error so the caller can handle the failed operation.
        throw bl::UnauthorizedError("Invalid username or password");
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This is a source comment that explains nearby code.
      // Success resets rate limiter counter
// This performs one step of the current operation, then continues to the next statement.
      state.rateLimiter.recordSuccess(username);
// This creates or updates a value that the following logic uses.
      auto token = state.sessions.createSession(username);

// This performs one step of the current operation, then continues to the next statement.
      state.audit.log(username, "LOGIN_SUCCESS", "Logged in successfully");

// This is part of the current expression or block; read it together with the nearby lines.
      res.set_content(
// This creates or updates a value that the following logic uses.
          json{{"token", token}, {"user", account->toJson()}}.dump(),
// This creates or updates a value that the following logic uses.
          "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── POST /api/auth/logout ──
// This creates or updates a value that the following logic uses.
  server.Post("/api/auth/logout", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                      httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This creates or updates a value that the following logic uses.
      auto token = extractToken(req);
// This controls which part of the program runs next, based on the current condition or result.
      if (!token.empty()) {
// This creates or updates a value that the following logic uses.
        auto username = state.sessions.validate(token);
// This controls which part of the program runs next, based on the current condition or result.
        if (!username.empty()) {
// This performs one step of the current operation, then continues to the next statement.
          state.audit.log(username, "LOGOUT", "Session invalidated");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        state.sessions.invalidate(token);
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This creates or updates a value that the following logic uses.
      res.set_content(json{{"message", "Logged out"}}.dump(),
// This creates or updates a value that the following logic uses.
                      "application/json");
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── GET /api/me ──
// This is part of the current expression or block; read it together with the nearby lines.
  server.Get("/api/me",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
             [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
               try {
// This creates or updates a value that the following logic uses.
                 auto *account = authenticate(req, state);
// This creates or updates a value that the following logic uses.
                 res.set_content(account->toJson().dump(), "application/json");
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               }
// This closes the block opened above, such as a function, class, or control-flow branch.
             });

// This is a source comment that explains nearby code.
  // ── PUT /api/donor/account ── (Edit name, phone, password with current pw)
// This creates or updates a value that the following logic uses.
  server.Put("/api/donor/account", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                       httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      checkPayloadSize(req);
// This creates or updates a value that the following logic uses.
      auto *account = authenticate(req, state);
// This creates or updates a value that the following logic uses.
      auto body = json::parse(req.body);

// This creates or updates a value that the following logic uses.
      auto currentPassword = body.at("currentPassword").get<std::string>();
// This controls which part of the program runs next, based on the current condition or result.
      if (!account->verifyPassword(currentPassword)) {
// This reports an error so the caller can handle the failed operation.
        throw bl::UnauthorizedError("Current password verification failed");
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This controls which part of the program runs next, based on the current condition or result.
      if (body.contains("fullName") &&
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
          !body["fullName"].get<std::string>().empty()) {
// This creates or updates a value that the following logic uses.
        account->setFullName(body["fullName"].get<std::string>());
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This controls which part of the program runs next, based on the current condition or result.
      if (body.contains("newPassword") &&
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
          !body["newPassword"].get<std::string>().empty()) {
// This creates or updates a value that the following logic uses.
        std::string newPw = body["newPassword"].get<std::string>();
// This controls which part of the program runs next, based on the current condition or result.
        if (newPw.length() < 3)
// This reports an error so the caller can handle the failed operation.
          throw bl::BadRequestError("New password must be at least 3 chars");
// This performs one step of the current operation, then continues to the next statement.
        account->updateCredential(bl::Credential::fromPassword(newPw));
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This performs one step of the current operation, then continues to the next statement.
      state.accounts.save();

// This is a source comment that explains nearby code.
      // Also update phone in donor profile if present
// This creates or updates a value that the following logic uses.
      auto donorRef = state.donors.find(account->username());
// This controls which part of the program runs next, based on the current condition or result.
      if (donorRef && body.contains("phone") &&
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
          !body["phone"].get<std::string>().empty()) {
// This creates or updates a value that the following logic uses.
        donorRef->get().setPhone(body["phone"].get<std::string>());
// This performs one step of the current operation, then continues to the next statement.
        state.donors.save();
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This is part of the current expression or block; read it together with the nearby lines.
      state.audit.log(account->username(), "PROFILE_UPDATE",
// This performs one step of the current operation, then continues to the next statement.
                      "Updated account credentials");
// This creates or updates a value that the following logic uses.
      res.set_content(json{{"message", "Account updated successfully"}}.dump(),
// This creates or updates a value that the following logic uses.
                      "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── GET /api/donor/profile ──
// This is part of the current expression or block; read it together with the nearby lines.
  server.Get("/api/donor/profile",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
             [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
               try {
// This creates or updates a value that the following logic uses.
                 auto *account = requireRole(req, state, "donor");
// This creates or updates a value that the following logic uses.
                 auto donorRef = state.donors.find(account->username());
// This controls which part of the program runs next, based on the current condition or result.
                 if (!donorRef) {
// This creates or updates a value that the following logic uses.
                   res.set_content("null", "application/json");
// This sends a result back to the code that called this function.
                   return;
// This closes the block opened above, such as a function, class, or control-flow branch.
                 }
// This is part of the current expression or block; read it together with the nearby lines.
                 res.set_content(donorRef->get().profileToJson(false).dump(),
// This creates or updates a value that the following logic uses.
                                 "application/json");
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               }
// This closes the block opened above, such as a function, class, or control-flow branch.
             });

// This is a source comment that explains nearby code.
  // ── PUT /api/donor/profile ──
// This creates or updates a value that the following logic uses.
  server.Put("/api/donor/profile", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                       httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      checkPayloadSize(req);
// This creates or updates a value that the following logic uses.
      auto *account = requireRole(req, state, "donor");
// This creates or updates a value that the following logic uses.
      auto body = json::parse(req.body);

// This creates or updates a value that the following logic uses.
      auto donorRef = state.donors.find(account->username());
// This controls which part of the program runs next, based on the current condition or result.
      if (donorRef) {
// This creates or updates a value that the following logic uses.
        auto &donor = donorRef->get();
// This creates or updates a value that the following logic uses.
        donor.setAge(body.at("age").get<int>());
// This creates or updates a value that the following logic uses.
        donor.setGender(body.at("gender").get<std::string>());
// This is part of the current expression or block; read it together with the nearby lines.
        donor.setBloodGroup(
// This creates or updates a value that the following logic uses.
            bl::bloodGroupFromString(body.at("bloodGroup").get<std::string>()));
// This creates or updates a value that the following logic uses.
        donor.setWeight(body.at("weight").get<double>());
// This creates or updates a value that the following logic uses.
        donor.setPhone(body.at("phone").get<std::string>());
// This controls which part of the program runs next, based on the current condition or result.
        if (body.at("consent").get<bool>())
// This performs one step of the current operation, then continues to the next statement.
          donor.grantConsent();
// This controls which part of the program runs next, based on the current condition or result.
        else
// This performs one step of the current operation, then continues to the next statement.
          donor.revokeConsent();
// This performs one step of the current operation, then continues to the next statement.
        donor.markProfileComplete();
// This closes the block opened above, such as a function, class, or control-flow branch.
      } else {
// This is part of the current expression or block; read it together with the nearby lines.
        bl::Donor donor(account->username(), account->fullName(),
// This performs one step of the current operation, then continues to the next statement.
                        bl::Credential{});
// This creates or updates a value that the following logic uses.
        donor.setAge(body.at("age").get<int>());
// This creates or updates a value that the following logic uses.
        donor.setGender(body.at("gender").get<std::string>());
// This is part of the current expression or block; read it together with the nearby lines.
        donor.setBloodGroup(
// This creates or updates a value that the following logic uses.
            bl::bloodGroupFromString(body.at("bloodGroup").get<std::string>()));
// This creates or updates a value that the following logic uses.
        donor.setWeight(body.at("weight").get<double>());
// This creates or updates a value that the following logic uses.
        donor.setPhone(body.at("phone").get<std::string>());
// This controls which part of the program runs next, based on the current condition or result.
        if (body.at("consent").get<bool>())
// This performs one step of the current operation, then continues to the next statement.
          donor.grantConsent();
// This controls which part of the program runs next, based on the current condition or result.
        else
// This performs one step of the current operation, then continues to the next statement.
          donor.revokeConsent();
// This performs one step of the current operation, then continues to the next statement.
        donor.markProfileComplete();
// This performs one step of the current operation, then continues to the next statement.
        state.donors.add(std::move(donor));
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This performs one step of the current operation, then continues to the next statement.
      state.donors.save();
// This is part of the current expression or block; read it together with the nearby lines.
      state.audit.log(account->username(), "DONOR_PROFILE_SAVE",
// This performs one step of the current operation, then continues to the next statement.
                      "Saved medical details");

// This creates or updates a value that the following logic uses.
      auto updatedRef = state.donors.find(account->username());
// This is part of the current expression or block; read it together with the nearby lines.
      res.set_content(updatedRef->get().profileToJson(false).dump(),
// This creates or updates a value that the following logic uses.
                      "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── GET /api/donor/eligibility ── (Includes 90/120 day waiting period check)
// This creates or updates a value that the following logic uses.
  server.Get("/api/donor/eligibility", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                           httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This creates or updates a value that the following logic uses.
      auto *account = requireRole(req, state, "donor");
// This creates or updates a value that the following logic uses.
      auto donorRef = state.donors.find(account->username());
// This controls which part of the program runs next, based on the current condition or result.
      if (!donorRef) {
// This reports an error so the caller can handle the failed operation.
        throw bl::BadRequestError("Please fill out your donor profile first");
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      const auto &donor = donorRef->get();
// This creates or updates a value that the following logic uses.
      std::optional<double> hb;
// This controls which part of the program runs next, based on the current condition or result.
      if (req.has_param("hb") && !req.get_param_value("hb").empty()) {
// This performs one step of the current operation, then continues to the next statement.
        hb = std::stod(req.get_param_value("hb"));
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      auto result = state.eligibility.evaluate(donor.age(), donor.gender(),
// This performs one step of the current operation, then continues to the next statement.
                                               donor.weight(), hb);

// This is a source comment that explains nearby code.
      // Add waiting period check
// This creates or updates a value that the following logic uses.
      bool waitPass = donor.isEligibleOnDate();
// This controls which part of the program runs next, based on the current condition or result.
      if (!waitPass) {
// This performs one step of the current operation, then continues to the next statement.
        result.eligible = false;
// This is part of the current expression or block; read it together with the nearby lines.
        result.checks.push_back({"Waiting Period", "fail",
// This is part of the current expression or block; read it together with the nearby lines.
                                 "Last donated on " + donor.lastDonationDate() +
// This is part of the current expression or block; read it together with the nearby lines.
                                     ". Next eligible date is " +
// This is part of the current expression or block; read it together with the nearby lines.
                                     donor.nextEligibleDate() + " (" +
// This is part of the current expression or block; read it together with the nearby lines.
                                     std::to_string(donor.daysUntilEligible()) +
// This performs one step of the current operation, then continues to the next statement.
                                     " days remaining)"});
// This closes the block opened above, such as a function, class, or control-flow branch.
      } else if (!donor.lastDonationDate().empty()) {
// This is part of the current expression or block; read it together with the nearby lines.
        result.checks.push_back(
// This is part of the current expression or block; read it together with the nearby lines.
            {"Waiting Period", "pass",
// This performs one step of the current operation, then continues to the next statement.
             "Waiting period fulfilled. Eligible to donate today."});
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      json checks = json::array();
// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &c : result.checks) {
// This is part of the current expression or block; read it together with the nearby lines.
        checks.push_back(
// This performs one step of the current operation, then continues to the next statement.
            {{"label", c.label}, {"status", c.status}, {"message", c.message}});
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      res.set_content(json{{"eligible", result.eligible},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"checks", checks},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"nextEligibleDate", donor.nextEligibleDate()},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"daysUntilEligible", donor.daysUntilEligible()}}
// This is part of the current expression or block; read it together with the nearby lines.
                          .dump(),
// This creates or updates a value that the following logic uses.
                      "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── GET /api/donor/card ── (Donor printable card & milestone badges)
// This creates or updates a value that the following logic uses.
  server.Get("/api/donor/card", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                    httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This creates or updates a value that the following logic uses.
      auto *account = requireRole(req, state, "donor");
// This creates or updates a value that the following logic uses.
      auto donorRef = state.donors.find(account->username());
// This controls which part of the program runs next, based on the current condition or result.
      if (!donorRef)
// This reports an error so the caller can handle the failed operation.
        throw bl::BadRequestError("Complete donor profile first");

// This creates or updates a value that the following logic uses.
      const auto &donor = donorRef->get();
// This creates or updates a value that the following logic uses.
      res.set_content(json{{"fullName", donor.fullName()},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"username", donor.username()},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"bloodGroup", donor.bloodGroupString()},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"donationCount", donor.donationCount()},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"lastDonationDate", donor.lastDonationDate()},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"nextEligibleDate", donor.nextEligibleDate()},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"milestones", donor.milestoneBadges()}}
// This is part of the current expression or block; read it together with the nearby lines.
                          .dump(),
// This creates or updates a value that the following logic uses.
                      "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── GET /api/donor/matching-requests ── (Find urgent requests matching
// This is a source comment that explains nearby code.
  // donor)
// This creates or updates a value that the following logic uses.
  server.Get("/api/donor/matching-requests", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                                 httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This creates or updates a value that the following logic uses.
      auto *account = requireRole(req, state, "donor");
// This creates or updates a value that the following logic uses.
      auto donorRef = state.donors.find(account->username());
// This controls which part of the program runs next, based on the current condition or result.
      if (!donorRef || !donorRef->get().bloodGroup()) {
// This creates or updates a value that the following logic uses.
        res.set_content("[]", "application/json");
// This sends a result back to the code that called this function.
        return;
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      const auto &donor = donorRef->get();
// This creates or updates a value that the following logic uses.
      json matches = json::array();

// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &requestPtr : state.requests.all()) {
// This controls which part of the program runs next, based on the current condition or result.
        if (requestPtr->status() != bl::RequestStatus::Open)
// This changes the current loop flow: stop this loop, or move to its next iteration.
          continue;

// This is a source comment that explains nearby code.
        // Compatible if donor can donate to the requested blood group
// This controls which part of the program runs next, based on the current condition or result.
        if (state.compatibility.canDonateTo(*donor.bloodGroup(),
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
                                            requestPtr->bloodGroup())) {
// This creates or updates a value that the following logic uses.
          auto j = requestPtr->toJson();
// This performs one step of the current operation, then continues to the next statement.
          j["isEligible"] = donor.isEligibleOnDate();

// This is a source comment that explains nearby code.
          // Check if this donor already pledged
// This creates or updates a value that the following logic uses.
          const auto &responders = requestPtr->responders();
// This is part of the current expression or block; read it together with the nearby lines.
          j["hasPledged"] =
// This is part of the current expression or block; read it together with the nearby lines.
              (std::find(responders.begin(), responders.end(),
// This performs one step of the current operation, then continues to the next statement.
                         account->username()) != responders.end());

// This is a source comment that explains nearby code.
          // Enrich with organization contact info
// This creates or updates a value that the following logic uses.
          auto *orgAcc =
// This performs one step of the current operation, then continues to the next statement.
              state.accounts.findByUsername(requestPtr->orgUsername());
// This controls which part of the program runs next, based on the current condition or result.
          if (orgAcc) {
// This creates or updates a value that the following logic uses.
            auto *org = dynamic_cast<const bl::Organization *>(orgAcc);
// This controls which part of the program runs next, based on the current condition or result.
            if (org) {
// This is part of the current expression or block; read it together with the nearby lines.
              j["orgPhone"] = !org->contactPhone().empty() ? org->contactPhone()
// This performs one step of the current operation, then continues to the next statement.
                                                           : "05942-235022";
// This is part of the current expression or block; read it together with the nearby lines.
              j["orgEmail"] = !org->contactEmail().empty()
// This is part of the current expression or block; read it together with the nearby lines.
                                  ? org->contactEmail()
// This performs one step of the current operation, then continues to the next statement.
                                  : "bloodbank@ramsayhospital.org";
// This performs one step of the current operation, then continues to the next statement.
              j["orgCity"] = org->city();
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This closes the block opened above, such as a function, class, or control-flow branch.
          }

// This performs one step of the current operation, then continues to the next statement.
          matches.push_back(j);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      res.set_content(matches.dump(), "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── GET /api/donor/appointments ── (Donor's upcoming booked camp slots)
// This is part of the current expression or block; read it together with the nearby lines.
  server.Get("/api/donor/appointments",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
             [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
               try {
// This creates or updates a value that the following logic uses.
                 auto *account = requireRole(req, state, "donor");
// This creates or updates a value that the following logic uses.
                 json appointments = json::array();

// This controls which part of the program runs next, based on the current condition or result.
                 for (const auto &camp : state.camps.all()) {
// This creates or updates a value that the following logic uses.
                   const auto &donors = camp.registeredDonors();
// This controls which part of the program runs next, based on the current condition or result.
                   if (std::find(donors.begin(), donors.end(),
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
                                 account->username()) != donors.end()) {
// This performs one step of the current operation, then continues to the next statement.
                     appointments.push_back(camp.toJson());
// This closes the block opened above, such as a function, class, or control-flow branch.
                   }
// This closes the block opened above, such as a function, class, or control-flow branch.
                 }

// This creates or updates a value that the following logic uses.
                 res.set_content(appointments.dump(), "application/json");
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               }
// This closes the block opened above, such as a function, class, or control-flow branch.
             });

// This is a source comment that explains nearby code.
  // ── GET /api/donor/pledged-requests ── (Urgent requests donor committed to)
// This creates or updates a value that the following logic uses.
  server.Get("/api/donor/pledged-requests", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                                httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This creates or updates a value that the following logic uses.
      auto *account = requireRole(req, state, "donor");
// This creates or updates a value that the following logic uses.
      json pledged = json::array();

// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &r : state.requests.all()) {
// This creates or updates a value that the following logic uses.
        const auto &responders = r->responders();
// This controls which part of the program runs next, based on the current condition or result.
        if (std::find(responders.begin(), responders.end(),
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
                      account->username()) != responders.end()) {
// This creates or updates a value that the following logic uses.
          auto rJson = r->toJson();
// This creates or updates a value that the following logic uses.
          auto *orgAcc = state.accounts.findByUsername(r->orgUsername());
// This controls which part of the program runs next, based on the current condition or result.
          if (orgAcc) {
// This creates or updates a value that the following logic uses.
            auto *org = dynamic_cast<const bl::Organization *>(orgAcc);
// This controls which part of the program runs next, based on the current condition or result.
            if (org) {
// This is part of the current expression or block; read it together with the nearby lines.
              rJson["orgPhone"] = !org->contactPhone().empty()
// This is part of the current expression or block; read it together with the nearby lines.
                                      ? org->contactPhone()
// This performs one step of the current operation, then continues to the next statement.
                                      : "05942-235022";
// This is part of the current expression or block; read it together with the nearby lines.
              rJson["orgEmail"] = !org->contactEmail().empty()
// This is part of the current expression or block; read it together with the nearby lines.
                                      ? org->contactEmail()
// This performs one step of the current operation, then continues to the next statement.
                                      : "bloodbank@ramsayhospital.org";
// This performs one step of the current operation, then continues to the next statement.
              rJson["orgCity"] = org->city();
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This closes the block opened above, such as a function, class, or control-flow branch.
          }
// This performs one step of the current operation, then continues to the next statement.
          pledged.push_back(rJson);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      res.set_content(pledged.dump(), "application/json");
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── GET /api/donor/history ──
// This is part of the current expression or block; read it together with the nearby lines.
  server.Get("/api/donor/history",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
             [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
               try {
// This creates or updates a value that the following logic uses.
                 auto *account = requireRole(req, state, "donor");
// This creates or updates a value that the following logic uses.
                 json records = json::array();
// This controls which part of the program runs next, based on the current condition or result.
                 for (const auto &d : state.donations.all()) {
// This controls which part of the program runs next, based on the current condition or result.
                   if (d.donorUsername() == account->username()) {
// This performs one step of the current operation, then continues to the next statement.
                     records.push_back(d.toJson());
// This closes the block opened above, such as a function, class, or control-flow branch.
                   }
// This closes the block opened above, such as a function, class, or control-flow branch.
                 }
// This creates or updates a value that the following logic uses.
                 res.set_content(records.dump(), "application/json");
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               }
// This closes the block opened above, such as a function, class, or control-flow branch.
             });

// This is a source comment that explains nearby code.
  // ── GET /api/camps ── (Public camps list)
// This is part of the current expression or block; read it together with the nearby lines.
  server.Get("/api/camps",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
             [&](const httplib::Request &, httplib::Response &res) {
// This creates or updates a value that the following logic uses.
               json camps = json::array();
// This controls which part of the program runs next, based on the current condition or result.
               for (const auto &c : state.camps.all()) {
// This performs one step of the current operation, then continues to the next statement.
                 camps.push_back(c.toJson());
// This closes the block opened above, such as a function, class, or control-flow branch.
               }
// This creates or updates a value that the following logic uses.
               res.set_content(camps.dump(), "application/json");
// This closes the block opened above, such as a function, class, or control-flow branch.
             });

// This is a source comment that explains nearby code.
  // ── GET /api/camps/:id ── (Camp detail view)
// This creates or updates a value that the following logic uses.
  server.Get(R"(/api/camps/(CAMP-\d+))", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                             httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This creates or updates a value that the following logic uses.
      std::string id = req.matches[1];
// This creates or updates a value that the following logic uses.
      auto campRef = state.camps.find(id);
// This controls which part of the program runs next, based on the current condition or result.
      if (!campRef)
// This reports an error so the caller can handle the failed operation.
        throw bl::NotFoundError("Camp not found");
// This creates or updates a value that the following logic uses.
      res.set_content(campRef->get().toJson().dump(), "application/json");
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── POST /api/camps ── (Create camp)
// This is part of the current expression or block; read it together with the nearby lines.
  server.Post(
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
      "/api/camps", [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
        try {
// This performs one step of the current operation, then continues to the next statement.
          checkPayloadSize(req);
// This creates or updates a value that the following logic uses.
          auto *account = requireRole(req, state, "ngo");
// This creates or updates a value that the following logic uses.
          auto *org = dynamic_cast<bl::Organization *>(account);
// This controls which part of the program runs next, based on the current condition or result.
          if (org && !org->isApproved()) {
// This reports an error so the caller can handle the failed operation.
            throw bl::ForbiddenError(
// This performs one step of the current operation, then continues to the next statement.
                "Organization account is pending admin approval.");
// This closes the block opened above, such as a function, class, or control-flow branch.
          }

// This creates or updates a value that the following logic uses.
          auto body = json::parse(req.body);
// This creates or updates a value that the following logic uses.
          auto name = body.at("name").get<std::string>();
// This creates or updates a value that the following logic uses.
          auto date = body.at("date").get<std::string>();
// This creates or updates a value that the following logic uses.
          int target = body.at("targetUnits").get<int>();
// This creates or updates a value that the following logic uses.
          int slotCap = body.value("slotCapacity", target * 2);

// This controls which part of the program runs next, based on the current condition or result.
          if (name.empty() || date.empty() || target <= 0) {
// This reports an error so the caller can handle the failed operation.
            throw bl::BadRequestError("Invalid camp details");
// This closes the block opened above, such as a function, class, or control-flow branch.
          }

// This creates or updates a value that the following logic uses.
          std::string campId =
// This performs one step of the current operation, then continues to the next statement.
              "CAMP-" + std::to_string(state.camps.all().size() + 1);
// This controls which part of the program runs next, based on the current condition or result.
          while (campId.size() < 8)
// This performs one step of the current operation, then continues to the next statement.
            campId.insert(5, "0");

// This performs one step of the current operation, then continues to the next statement.
          state.camps.add(bl::Camp(campId, name, date, target, slotCap));
// This performs one step of the current operation, then continues to the next statement.
          state.camps.save();

// This is part of the current expression or block; read it together with the nearby lines.
          state.audit.log(account->username(), "CAMP_CREATE",
// This performs one step of the current operation, then continues to the next statement.
                          "Created camp " + campId + " (" + name + ")");

// This performs one step of the current operation, then continues to the next statement.
          res.status = 201;
// This is part of the current expression or block; read it together with the nearby lines.
          res.set_content(state.camps.all().back().toJson().dump(),
// This creates or updates a value that the following logic uses.
                          "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
        } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
          handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
        } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
          handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
      });

// This is a source comment that explains nearby code.
  // ── PUT /api/camps/:id ── (Edit date and target before start)
// This creates or updates a value that the following logic uses.
  server.Put(R"(/api/camps/(CAMP-\d+))", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                             httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      checkPayloadSize(req);
// This creates or updates a value that the following logic uses.
      auto *account = requireRole(req, state, "ngo");
// This creates or updates a value that the following logic uses.
      std::string id = req.matches[1];
// This creates or updates a value that the following logic uses.
      auto campRef = state.camps.find(id);
// This controls which part of the program runs next, based on the current condition or result.
      if (!campRef)
// This reports an error so the caller can handle the failed operation.
        throw bl::NotFoundError("Camp not found");

// This creates or updates a value that the following logic uses.
      auto body = json::parse(req.body);
// This creates or updates a value that the following logic uses.
      std::string newDate = body.at("date").get<std::string>();
// This creates or updates a value that the following logic uses.
      int newTarget = body.at("targetUnits").get<int>();

// This performs one step of the current operation, then continues to the next statement.
      campRef->get().editDetails(newDate, newTarget);
// This performs one step of the current operation, then continues to the next statement.
      state.camps.save();

// This is part of the current expression or block; read it together with the nearby lines.
      state.audit.log(account->username(), "CAMP_EDIT",
// This performs one step of the current operation, then continues to the next statement.
                      "Edited date/target for " + id);
// This creates or updates a value that the following logic uses.
      res.set_content(campRef->get().toJson().dump(), "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── PUT /api/camps/:id/status ── (State machine status change)
// This creates or updates a value that the following logic uses.
  server.Put(R"(/api/camps/(CAMP-\d+)/status)", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                                    httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      checkPayloadSize(req);
// This creates or updates a value that the following logic uses.
      auto *account = requireRole(req, state, "ngo");
// This creates or updates a value that the following logic uses.
      std::string id = req.matches[1];
// This creates or updates a value that the following logic uses.
      auto campRef = state.camps.find(id);
// This controls which part of the program runs next, based on the current condition or result.
      if (!campRef)
// This reports an error so the caller can handle the failed operation.
        throw bl::NotFoundError("Camp not found");

// This creates or updates a value that the following logic uses.
      auto body = json::parse(req.body);
// This creates or updates a value that the following logic uses.
      std::string statusStr = body.at("status").get<std::string>();
// This performs one step of the current operation, then continues to the next statement.
      bl::CampStatus newStatus = bl::campStatusFromString(statusStr);

// This performs one step of the current operation, then continues to the next statement.
      campRef->get().setStatus(newStatus);
// This performs one step of the current operation, then continues to the next statement.
      state.camps.save();

// This is part of the current expression or block; read it together with the nearby lines.
      state.audit.log(account->username(), "CAMP_STATUS",
// This performs one step of the current operation, then continues to the next statement.
                      "Changed " + id + " to " + statusStr);
// This creates or updates a value that the following logic uses.
      res.set_content(campRef->get().toJson().dump(), "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── POST /api/camps/:id/register ── (Donor registers for camp slot)
// This is part of the current expression or block; read it together with the nearby lines.
  server.Post(R"(/api/camps/(CAMP-\d+)/register)",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
              [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
                try {
// This creates or updates a value that the following logic uses.
                  auto *account = requireRole(req, state, "donor");
// This creates or updates a value that the following logic uses.
                  std::string id = req.matches[1];
// This creates or updates a value that the following logic uses.
                  auto campRef = state.camps.find(id);
// This controls which part of the program runs next, based on the current condition or result.
                  if (!campRef)
// This reports an error so the caller can handle the failed operation.
                    throw bl::NotFoundError("Camp not found");

// This performs one step of the current operation, then continues to the next statement.
                  campRef->get().registerDonor(account->username());
// This performs one step of the current operation, then continues to the next statement.
                  state.camps.save();

// This is part of the current expression or block; read it together with the nearby lines.
                  state.audit.log(account->username(), "CAMP_REGISTER",
// This performs one step of the current operation, then continues to the next statement.
                                  "Registered for slot at " + id);
// This is part of the current expression or block; read it together with the nearby lines.
                  state.notifications.send(
// This is part of the current expression or block; read it together with the nearby lines.
                      account->username(), "Camp Registration Confirmed",
// This is part of the current expression or block; read it together with the nearby lines.
                      "You have registered for " + campRef->get().name() +
// This performs one step of the current operation, then continues to the next statement.
                          " on " + campRef->get().date());

// This is part of the current expression or block; read it together with the nearby lines.
                  res.set_content(
// This creates or updates a value that the following logic uses.
                      json{{"message", "Successfully registered for camp"},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"camp", campRef->get().toJson()}}
// This is part of the current expression or block; read it together with the nearby lines.
                          .dump(),
// This creates or updates a value that the following logic uses.
                      "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
                } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
                  handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
                } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
                  handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
                }
// This closes the block opened above, such as a function, class, or control-flow branch.
              });

// This is a source comment that explains nearby code.
  // ── DELETE /api/camps/:id/register ── (Donor unregisters / cancels slot)
// This is part of the current expression or block; read it together with the nearby lines.
  server.Delete(
// This is part of the current expression or block; read it together with the nearby lines.
      R"(/api/camps/(CAMP-\d+)/register)",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
      [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
        try {
// This creates or updates a value that the following logic uses.
          auto *account = requireRole(req, state, "donor");
// This creates or updates a value that the following logic uses.
          std::string id = req.matches[1];
// This creates or updates a value that the following logic uses.
          auto campRef = state.camps.find(id);
// This controls which part of the program runs next, based on the current condition or result.
          if (!campRef)
// This reports an error so the caller can handle the failed operation.
            throw bl::NotFoundError("Camp not found");

// This performs one step of the current operation, then continues to the next statement.
          campRef->get().unregisterDonor(account->username());
// This performs one step of the current operation, then continues to the next statement.
          state.camps.save();

// This is part of the current expression or block; read it together with the nearby lines.
          state.audit.log(account->username(), "CAMP_UNREGISTER",
// This performs one step of the current operation, then continues to the next statement.
                          "Cancelled slot reservation at " + id);
// This is part of the current expression or block; read it together with the nearby lines.
          state.notifications.send(
// This is part of the current expression or block; read it together with the nearby lines.
              account->username(), "Camp Reservation Cancelled",
// This performs one step of the current operation, then continues to the next statement.
              "Your slot at " + campRef->get().name() + " has been cancelled.");

// This is part of the current expression or block; read it together with the nearby lines.
          res.set_content(
// This creates or updates a value that the following logic uses.
              json{{"message", "Successfully cancelled camp reservation"},
// This is part of the current expression or block; read it together with the nearby lines.
                   {"camp", campRef->get().toJson()}}
// This is part of the current expression or block; read it together with the nearby lines.
                  .dump(),
// This creates or updates a value that the following logic uses.
              "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
        } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
          handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
        } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
          handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
      });

// This is a source comment that explains nearby code.
  // ── GET /api/org/camps/:id/donors ── (NGO views registered donor roster for
// This is a source comment that explains nearby code.
  // a camp)
// This is part of the current expression or block; read it together with the nearby lines.
  server.Get(
// This is part of the current expression or block; read it together with the nearby lines.
      R"(/api/org/camps/(CAMP-\d+)/donors)",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
      [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
        try {
// This performs one step of the current operation, then continues to the next statement.
          requireRole(req, state, "ngo");
// This creates or updates a value that the following logic uses.
          std::string campId = req.matches[1];
// This creates or updates a value that the following logic uses.
          auto campRef = state.camps.find(campId);
// This controls which part of the program runs next, based on the current condition or result.
          if (!campRef)
// This reports an error so the caller can handle the failed operation.
            throw bl::NotFoundError("Camp not found");

// This creates or updates a value that the following logic uses.
          json roster = json::array();
// This controls which part of the program runs next, based on the current condition or result.
          for (const auto &uname : campRef->get().registeredDonors()) {
// This creates or updates a value that the following logic uses.
            auto dRef = state.donors.find(uname);
// This controls which part of the program runs next, based on the current condition or result.
            if (dRef) {
// This creates or updates a value that the following logic uses.
              const auto &d = dRef->get();
// This is part of the current expression or block; read it together with the nearby lines.
              roster.push_back({{"username", d.username()},
// This is part of the current expression or block; read it together with the nearby lines.
                                {"fullName", d.fullName()},
// This is part of the current expression or block; read it together with the nearby lines.
                                {"bloodGroup", d.bloodGroupString()},
// This is part of the current expression or block; read it together with the nearby lines.
                                {"phone", d.phone()},
// This is part of the current expression or block; read it together with the nearby lines.
                                {"age", d.age()},
// This is part of the current expression or block; read it together with the nearby lines.
                                {"weight", d.weight()},
// This is part of the current expression or block; read it together with the nearby lines.
                                {"isEligibleToday", d.isEligibleOnDate()},
// This performs one step of the current operation, then continues to the next statement.
                                {"lastDonationDate", d.lastDonationDate()}});
// This closes the block opened above, such as a function, class, or control-flow branch.
            } else {
// This creates or updates a value that the following logic uses.
              auto *acc = state.accounts.findByUsername(uname);
// This is part of the current expression or block; read it together with the nearby lines.
              roster.push_back({{"username", uname},
// This is part of the current expression or block; read it together with the nearby lines.
                                {"fullName", acc ? acc->fullName() : uname},
// This is part of the current expression or block; read it together with the nearby lines.
                                {"bloodGroup", "Unknown"},
// This is part of the current expression or block; read it together with the nearby lines.
                                {"phone", "N/A"},
// This is part of the current expression or block; read it together with the nearby lines.
                                {"isEligibleToday", true},
// This performs one step of the current operation, then continues to the next statement.
                                {"lastDonationDate", ""}});
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This closes the block opened above, such as a function, class, or control-flow branch.
          }

// This creates or updates a value that the following logic uses.
          res.set_content(json{{"campId", campId},
// This is part of the current expression or block; read it together with the nearby lines.
                               {"campName", campRef->get().name()},
// This is part of the current expression or block; read it together with the nearby lines.
                               {"campDate", campRef->get().date()},
// This is part of the current expression or block; read it together with the nearby lines.
                               {"donors", roster}}
// This is part of the current expression or block; read it together with the nearby lines.
                              .dump(),
// This creates or updates a value that the following logic uses.
                          "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
        } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
          handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
        } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
          handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
      });

// This is a source comment that explains nearby code.
  // ── POST /api/donations ── (Records donation; checks waiting period)
// This creates or updates a value that the following logic uses.
  server.Post("/api/donations", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                    httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      checkPayloadSize(req);
// This creates or updates a value that the following logic uses.
      auto *account = requireRole(req, state, "ngo");
// This creates or updates a value that the following logic uses.
      auto *org = dynamic_cast<bl::Organization *>(account);
// This controls which part of the program runs next, based on the current condition or result.
      if (org && !org->isApproved()) {
// This reports an error so the caller can handle the failed operation.
        throw bl::ForbiddenError("Organization account is pending approval.");
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      auto body = json::parse(req.body);
// This creates or updates a value that the following logic uses.
      auto username = body.at("username").get<std::string>();
// This creates or updates a value that the following logic uses.
      auto campId = body.at("campId").get<std::string>();
// This creates or updates a value that the following logic uses.
      auto date = body.at("date").get<std::string>();
// This creates or updates a value that the following logic uses.
      auto status = body.at("status").get<std::string>();

// This controls which part of the program runs next, based on the current condition or result.
      if (status != "Completed" && status != "Deferred") {
// This reports an error so the caller can handle the failed operation.
        throw bl::BadRequestError("Status must be 'Completed' or 'Deferred'");
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      auto donorRef = state.donors.find(username);
// This controls which part of the program runs next, based on the current condition or result.
      if (!donorRef)
// This reports an error so the caller can handle the failed operation.
        throw bl::NotFoundError("Donor username not found in database");

// This creates or updates a value that the following logic uses.
      auto campRef = state.camps.find(campId);
// This controls which part of the program runs next, based on the current condition or result.
      if (!campRef)
// This reports an error so the caller can handle the failed operation.
        throw bl::NotFoundError("Camp not found");

// This creates or updates a value that the following logic uses.
      auto &donor = donorRef->get();

// This is a source comment that explains nearby code.
      // Donor safety check: waiting period!
// This controls which part of the program runs next, based on the current condition or result.
      if (status == "Completed" && !donor.isEligibleOnDate(date)) {
// This reports an error so the caller can handle the failed operation.
        throw bl::BadRequestError(
// This is part of the current expression or block; read it together with the nearby lines.
            "Safety Alert: Donor last donated on " + donor.lastDonationDate() +
// This performs one step of the current operation, then continues to the next statement.
            ". Next eligible date is " + donor.nextEligibleDate() + ".");
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      std::string donId =
// This performs one step of the current operation, then continues to the next statement.
          "R-" + std::to_string(2200 + state.donations.all().size() + 1);
// This creates or updates a value that the following logic uses.
      auto donation = bl::Donation::record(donId, donor, campId,
// This performs one step of the current operation, then continues to the next statement.
                                           campRef->get().name(), date, status);

// This performs one step of the current operation, then continues to the next statement.
      state.donations.add(std::move(donation));

// This controls which part of the program runs next, based on the current condition or result.
      if (status == "Completed") {
// This performs one step of the current operation, then continues to the next statement.
        campRef->get().addUnit();
// This performs one step of the current operation, then continues to the next statement.
        donor.incrementDonationCount(date);
// This performs one step of the current operation, then continues to the next statement.
        state.donors.save();
// This closes the block opened above, such as a function, class, or control-flow branch.
      } else {
// This performs one step of the current operation, then continues to the next statement.
        campRef->get().addDeferred();
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This performs one step of the current operation, then continues to the next statement.
      state.camps.save();
// This performs one step of the current operation, then continues to the next statement.
      state.donations.save();

// This is part of the current expression or block; read it together with the nearby lines.
      state.audit.log(account->username(), "DONATION_RECORD",
// This is part of the current expression or block; read it together with the nearby lines.
                      "Recorded " + status + " donation for " + username +
// This performs one step of the current operation, then continues to the next statement.
                          " at " + campId);

// This performs one step of the current operation, then continues to the next statement.
      res.status = 201;
// This is part of the current expression or block; read it together with the nearby lines.
      res.set_content(state.donations.all().back().toJson().dump(),
// This creates or updates a value that the following logic uses.
                      "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── GET /api/inventory/stock ── (Org / Admin: blood stock & 42-day expiry)
// This creates or updates a value that the following logic uses.
  server.Get("/api/inventory/stock", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                         httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      requireRole(req, state, "ngo");

// This creates or updates a value that the following logic uses.
      std::string today = bl::DateUtil::today();
// This creates or updates a value that the following logic uses.
      std::map<std::string, int> usableUnits;
// This creates or updates a value that the following logic uses.
      std::map<std::string, int> expiredUnits;

// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &bg :
// This is part of the current expression or block; read it together with the nearby lines.
           {bl::BloodGroup::APos, bl::BloodGroup::ANeg, bl::BloodGroup::BPos,
// This is part of the current expression or block; read it together with the nearby lines.
            bl::BloodGroup::BNeg, bl::BloodGroup::ABPos, bl::BloodGroup::ABNeg,
// This opens a scope for the related statements that follow.
            bl::BloodGroup::OPos, bl::BloodGroup::ONeg}) {
// This performs one step of the current operation, then continues to the next statement.
        usableUnits[bl::bloodGroupToString(bg)] = 0;
// This performs one step of the current operation, then continues to the next statement.
        expiredUnits[bl::bloodGroupToString(bg)] = 0;
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      json activeUnitList = json::array();
// This creates or updates a value that the following logic uses.
      json expiredUnitList = json::array();

// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &don : state.donations.all()) {
// This controls which part of the program runs next, based on the current condition or result.
        if (don.status() != "Completed" && don.status() != "Discarded" &&
// This is part of the current expression or block; read it together with the nearby lines.
            don.status() != "Transfused")
// This changes the current loop flow: stop this loop, or move to its next iteration.
          continue;
// This is part of the current expression or block; read it together with the nearby lines.
        bl::BloodUnit unit(don.id(), bl::bloodGroupFromString(don.bloodGroup()),
// This performs one step of the current operation, then continues to the next statement.
                           don.date());
// This creates or updates a value that the following logic uses.
        auto uJson = unit.toJson(today);
// This performs one step of the current operation, then continues to the next statement.
        uJson["campName"] = don.campName();
// This performs one step of the current operation, then continues to the next statement.
        uJson["donorName"] = don.donorName();
// This performs one step of the current operation, then continues to the next statement.
        uJson["donorUsername"] = don.donorUsername();
// This performs one step of the current operation, then continues to the next statement.
        uJson["donationStatus"] = don.status();

// This controls which part of the program runs next, based on the current condition or result.
        if (don.status() == "Discarded") {
// This performs one step of the current operation, then continues to the next statement.
          uJson["viability"] = "Discarded";
// This performs one step of the current operation, then continues to the next statement.
          expiredUnitList.push_back(uJson);
// This closes the block opened above, such as a function, class, or control-flow branch.
        } else if (don.status() == "Transfused") {
// This performs one step of the current operation, then continues to the next statement.
          uJson["viability"] = "Transfused";
// This closes the block opened above, such as a function, class, or control-flow branch.
        } else if (unit.isExpired(today)) {
// This performs one step of the current operation, then continues to the next statement.
          uJson["viability"] = "Expired";
// This performs one step of the current operation, then continues to the next statement.
          expiredUnits[unit.bloodGroupString()]++;
// This performs one step of the current operation, then continues to the next statement.
          expiredUnitList.push_back(uJson);
// This closes the block opened above, such as a function, class, or control-flow branch.
        } else {
// This is part of the current expression or block; read it together with the nearby lines.
          uJson["viability"] =
// This performs one step of the current operation, then continues to the next statement.
              unit.daysRemaining(today) <= 7 ? "Expiring Soon" : "Fresh";
// This performs one step of the current operation, then continues to the next statement.
          usableUnits[unit.bloodGroupString()]++;
// This performs one step of the current operation, then continues to the next statement.
          activeUnitList.push_back(uJson);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      json summary = json::object();
// This creates or updates a value that the following logic uses.
      const int LOW_THRESHOLD = 3;
// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &[bg, count] : usableUnits) {
// This is part of the current expression or block; read it together with the nearby lines.
        summary[bg] = {{"count", count},
// This is part of the current expression or block; read it together with the nearby lines.
                       {"isLow", count < LOW_THRESHOLD},
// This performs one step of the current operation, then continues to the next statement.
                       {"expiredCount", expiredUnits[bg]}};
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      res.set_content(json{{"today", today},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"threshold", LOW_THRESHOLD},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"stockByGroup", summary},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"activeUnits", activeUnitList},
// This is part of the current expression or block; read it together with the nearby lines.
                           {"expiredUnits", expiredUnitList}}
// This is part of the current expression or block; read it together with the nearby lines.
                          .dump(),
// This creates or updates a value that the following logic uses.
                      "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── PUT /api/inventory/units/:id/status ── (Discard expired unit or mark
// This is a source comment that explains nearby code.
  // transfused)
// This is part of the current expression or block; read it together with the nearby lines.
  server.Put(R"(/api/inventory/units/([A-Za-z0-9-]+)/status)",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
             [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
               try {
// This performs one step of the current operation, then continues to the next statement.
                 checkPayloadSize(req);
// This creates or updates a value that the following logic uses.
                 auto *account = requireRole(req, state, "ngo");
// This creates or updates a value that the following logic uses.
                 std::string unitId = req.matches[1];
// This creates or updates a value that the following logic uses.
                 auto donRef = state.donations.find(unitId);
// This controls which part of the program runs next, based on the current condition or result.
                 if (!donRef)
// This reports an error so the caller can handle the failed operation.
                   throw bl::NotFoundError("Blood unit record not found");

// This creates or updates a value that the following logic uses.
                 auto body = json::parse(req.body);
// This creates or updates a value that the following logic uses.
                 std::string newStatus = body.at("status").get<std::string>();
// This controls which part of the program runs next, based on the current condition or result.
                 if (newStatus != "Discarded" && newStatus != "Transfused" &&
// This opens a scope for the related statements that follow.
                     newStatus != "Completed") {
// This reports an error so the caller can handle the failed operation.
                   throw bl::BadRequestError(
// This performs one step of the current operation, then continues to the next statement.
                       "Status must be Discarded, Transfused, or Completed");
// This closes the block opened above, such as a function, class, or control-flow branch.
                 }

// This performs one step of the current operation, then continues to the next statement.
                 donRef->get().setStatus(newStatus);
// This performs one step of the current operation, then continues to the next statement.
                 state.donations.save();

// This is part of the current expression or block; read it together with the nearby lines.
                 state.audit.log(account->username(), "INVENTORY_DISPOSAL",
// This performs one step of the current operation, then continues to the next statement.
                                 "Updated unit " + unitId + " to " + newStatus);

// This is part of the current expression or block; read it together with the nearby lines.
                 res.set_content(
// This creates or updates a value that the following logic uses.
                     json{{"message", "Blood unit status updated successfully"},
// This is part of the current expression or block; read it together with the nearby lines.
                          {"unitId", unitId},
// This is part of the current expression or block; read it together with the nearby lines.
                          {"status", newStatus}}
// This is part of the current expression or block; read it together with the nearby lines.
                         .dump(),
// This creates or updates a value that the following logic uses.
                     "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               }
// This closes the block opened above, such as a function, class, or control-flow branch.
             });

// This is a source comment that explains nearby code.
  // ── GET /api/public/stock ── (Public view: Low / Adequate status only)
// This creates or updates a value that the following logic uses.
  server.Get("/api/public/stock", [&](const httplib::Request &,
// This opens a scope for the related statements that follow.
                                      httplib::Response &res) {
// This creates or updates a value that the following logic uses.
    std::string today = bl::DateUtil::today();
// This creates or updates a value that the following logic uses.
    std::map<std::string, int> usableUnits;

// This controls which part of the program runs next, based on the current condition or result.
    for (const auto &bg :
// This is part of the current expression or block; read it together with the nearby lines.
         {bl::BloodGroup::APos, bl::BloodGroup::ANeg, bl::BloodGroup::BPos,
// This is part of the current expression or block; read it together with the nearby lines.
          bl::BloodGroup::BNeg, bl::BloodGroup::ABPos, bl::BloodGroup::ABNeg,
// This opens a scope for the related statements that follow.
          bl::BloodGroup::OPos, bl::BloodGroup::ONeg}) {
// This performs one step of the current operation, then continues to the next statement.
      usableUnits[bl::bloodGroupToString(bg)] = 0;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This controls which part of the program runs next, based on the current condition or result.
    for (const auto &don : state.donations.all()) {
// This controls which part of the program runs next, based on the current condition or result.
      if (don.status() != "Completed")
// This changes the current loop flow: stop this loop, or move to its next iteration.
        continue;
// This is part of the current expression or block; read it together with the nearby lines.
      bl::BloodUnit unit(don.id(), bl::bloodGroupFromString(don.bloodGroup()),
// This performs one step of the current operation, then continues to the next statement.
                         don.date());
// This controls which part of the program runs next, based on the current condition or result.
      if (!unit.isExpired(today)) {
// This performs one step of the current operation, then continues to the next statement.
        usableUnits[unit.bloodGroupString()]++;
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This creates or updates a value that the following logic uses.
    json publicStock = json::object();
// This controls which part of the program runs next, based on the current condition or result.
    for (const auto &[bg, count] : usableUnits) {
// This is part of the current expression or block; read it together with the nearby lines.
      publicStock[bg] = {{"status", count < 3 ? "Low" : "Adequate"},
// This performs one step of the current operation, then continues to the next statement.
                         {"count", count}};
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This creates or updates a value that the following logic uses.
    res.set_content(publicStock.dump(), "application/json");
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── GET /api/requests ── (Lists open blood requests prioritized)
// This is part of the current expression or block; read it together with the nearby lines.
  server.Get("/api/requests",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
             [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
               try {
// This creates or updates a value that the following logic uses.
                 std::vector<const bl::Request *> sortedReqs;
// This controls which part of the program runs next, based on the current condition or result.
                 for (const auto &r : state.requests.all()) {
// This performs one step of the current operation, then continues to the next statement.
                   sortedReqs.push_back(r.get());
// This closes the block opened above, such as a function, class, or control-flow branch.
                 }

// This is a source comment that explains nearby code.
                 // Polymorphic sorting using RequestPrioritizer
// This performs one step of the current operation, then continues to the next statement.
                 bl::RequestPrioritizer::sort(sortedReqs);

// This creates or updates a value that the following logic uses.
                 json list = json::array();
// This controls which part of the program runs next, based on the current condition or result.
                 for (const auto *r : sortedReqs) {
// This performs one step of the current operation, then continues to the next statement.
                   list.push_back(r->toJson());
// This closes the block opened above, such as a function, class, or control-flow branch.
                 }

// This creates or updates a value that the following logic uses.
                 res.set_content(list.dump(), "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               }
// This closes the block opened above, such as a function, class, or control-flow branch.
             });

// This is a source comment that explains nearby code.
  // ── POST /api/requests ── (Orgs create Urgent or Routine requests)
// This creates or updates a value that the following logic uses.
  server.Post("/api/requests", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                   httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      checkPayloadSize(req);
// This creates or updates a value that the following logic uses.
      auto *account = requireRole(req, state, "ngo");
// This creates or updates a value that the following logic uses.
      auto body = json::parse(req.body);

// This creates or updates a value that the following logic uses.
      auto bgStr = body.at("bloodGroup").get<std::string>();
// This creates or updates a value that the following logic uses.
      int units = body.at("unitsNeeded").get<int>();
// This creates or updates a value that the following logic uses.
      std::string hospital = body.value("hospitalName", account->fullName());
// This creates or updates a value that the following logic uses.
      std::string urgency = body.value("urgency", "Normal");

// This controls which part of the program runs next, based on the current condition or result.
      if (units <= 0)
// This reports an error so the caller can handle the failed operation.
        throw bl::BadRequestError("Units needed must be positive");

// This creates or updates a value that the following logic uses.
      std::string reqId =
// This performs one step of the current operation, then continues to the next statement.
          "REQ-" + std::to_string(100 + state.requests.all().size() + 1);
// This performs one step of the current operation, then continues to the next statement.
      bl::BloodGroup bg = bl::bloodGroupFromString(bgStr);

// This creates or updates a value that the following logic uses.
      std::unique_ptr<bl::Request> newReq;
// This controls which part of the program runs next, based on the current condition or result.
      if (urgency == "Critical") {
// This is part of the current expression or block; read it together with the nearby lines.
        newReq = std::make_unique<bl::UrgentRequest>(reqId, account->username(),
// This performs one step of the current operation, then continues to the next statement.
                                                     hospital, bg, units, true);
// This closes the block opened above, such as a function, class, or control-flow branch.
      } else if (urgency == "Urgent") {
// This is part of the current expression or block; read it together with the nearby lines.
        newReq = std::make_unique<bl::UrgentRequest>(
// This performs one step of the current operation, then continues to the next statement.
            reqId, account->username(), hospital, bg, units, false);
// This closes the block opened above, such as a function, class, or control-flow branch.
      } else {
// This is part of the current expression or block; read it together with the nearby lines.
        newReq = std::make_unique<bl::RoutineRequest>(
// This performs one step of the current operation, then continues to the next statement.
            reqId, account->username(), hospital, bg, units);
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This is a source comment that explains nearby code.
      // Design Pattern: Strategy Matching & Notification
// This creates or updates a value that the following logic uses.
      auto matchedDonors =
// This performs one step of the current operation, then continues to the next statement.
          state.matchingEngine.findMatches(state.donors.all(), *newReq);
// This controls which part of the program runs next, based on the current condition or result.
      for (const auto *d : matchedDonors) {
// This is part of the current expression or block; read it together with the nearby lines.
        state.notifications.send(d->username(), "Urgent Blood Need Alert",
// This is part of the current expression or block; read it together with the nearby lines.
                                 "BloodLine: " + hospital +
// This is part of the current expression or block; read it together with the nearby lines.
                                     " has an urgent request for " + bgStr +
// This performs one step of the current operation, then continues to the next statement.
                                     " blood.");
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This performs one step of the current operation, then continues to the next statement.
      state.requests.add(std::move(newReq));
// This performs one step of the current operation, then continues to the next statement.
      state.requests.save();

// This is part of the current expression or block; read it together with the nearby lines.
      state.audit.log(account->username(), "REQUEST_CREATE",
// This is part of the current expression or block; read it together with the nearby lines.
                      "Created " + urgency + " request " + reqId + " for " +
// This performs one step of the current operation, then continues to the next statement.
                          bgStr);

// This performs one step of the current operation, then continues to the next statement.
      res.status = 201;
// This is part of the current expression or block; read it together with the nearby lines.
      res.set_content(state.requests.all().back()->toJson().dump(),
// This creates or updates a value that the following logic uses.
                      "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── POST /api/requests/:id/respond ── (Donor pledges to donate)
// This is part of the current expression or block; read it together with the nearby lines.
  server.Post(R"(/api/requests/(REQ-\d+)/respond)",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
              [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
                try {
// This creates or updates a value that the following logic uses.
                  auto *account = requireRole(req, state, "donor");
// This creates or updates a value that the following logic uses.
                  std::string reqId = req.matches[1];
// This creates or updates a value that the following logic uses.
                  auto *r = state.requests.find(reqId);
// This controls which part of the program runs next, based on the current condition or result.
                  if (!r)
// This reports an error so the caller can handle the failed operation.
                    throw bl::NotFoundError("Blood request not found");

// This performs one step of the current operation, then continues to the next statement.
                  r->addResponder(account->username());
// This performs one step of the current operation, then continues to the next statement.
                  state.requests.save();

// This is part of the current expression or block; read it together with the nearby lines.
                  state.audit.log(account->username(), "REQUEST_PLEDGE",
// This performs one step of the current operation, then continues to the next statement.
                                  "Pledged to donate for " + reqId);
// This is part of the current expression or block; read it together with the nearby lines.
                  state.notifications.send(
// This is part of the current expression or block; read it together with the nearby lines.
                      r->orgUsername(), "Donor Response Received",
// This is part of the current expression or block; read it together with the nearby lines.
                      "Donor @" + account->username() +
// This performs one step of the current operation, then continues to the next statement.
                          " pledged to donate for " + reqId);

// This creates or updates a value that the following logic uses.
                  res.set_content(r->toJson().dump(), "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
                } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
                  handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
                } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
                  handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
                }
// This closes the block opened above, such as a function, class, or control-flow branch.
              });

// This is a source comment that explains nearby code.
  // ── PUT /api/requests/:id/status ── (Fulfill or close request)
// This is part of the current expression or block; read it together with the nearby lines.
  server.Put(R"(/api/requests/(REQ-\d+)/status)",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
             [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
               try {
// This performs one step of the current operation, then continues to the next statement.
                 checkPayloadSize(req);
// This creates or updates a value that the following logic uses.
                 auto *account = requireRole(req, state, "ngo");
// This creates or updates a value that the following logic uses.
                 std::string reqId = req.matches[1];
// This creates or updates a value that the following logic uses.
                 auto *r = state.requests.find(reqId);
// This controls which part of the program runs next, based on the current condition or result.
                 if (!r)
// This reports an error so the caller can handle the failed operation.
                   throw bl::NotFoundError("Request not found");

// This creates or updates a value that the following logic uses.
                 auto body = json::parse(req.body);
// This creates or updates a value that the following logic uses.
                 std::string st = body.at("status").get<std::string>();
// This controls which part of the program runs next, based on the current condition or result.
                 if (st == "Fulfilled")
// This performs one step of the current operation, then continues to the next statement.
                   r->markFulfilled();
// This controls which part of the program runs next, based on the current condition or result.
                 else if (st == "Closed")
// This performs one step of the current operation, then continues to the next statement.
                   r->markClosed();
// This controls which part of the program runs next, based on the current condition or result.
                 else
// This reports an error so the caller can handle the failed operation.
                   throw bl::BadRequestError(
// This performs one step of the current operation, then continues to the next statement.
                       "Status must be Fulfilled or Closed");

// This performs one step of the current operation, then continues to the next statement.
                 state.requests.save();
// This is part of the current expression or block; read it together with the nearby lines.
                 state.audit.log(account->username(), "REQUEST_STATUS",
// This performs one step of the current operation, then continues to the next statement.
                                 "Updated " + reqId + " to " + st);

// This creates or updates a value that the following logic uses.
                 res.set_content(r->toJson().dump(), "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               }
// This closes the block opened above, such as a function, class, or control-flow branch.
             });

// This is a source comment that explains nearby code.
  // ── GET /api/org/requests/:id/responders ── (NGO views donor responders list
// This is a source comment that explains nearby code.
  // with phone numbers)
// This is part of the current expression or block; read it together with the nearby lines.
  server.Get(R"(/api/org/requests/(REQ-\d+)/responders)",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
             [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
               try {
// This performs one step of the current operation, then continues to the next statement.
                 requireRole(req, state, "ngo");
// This creates or updates a value that the following logic uses.
                 std::string reqId = req.matches[1];
// This creates or updates a value that the following logic uses.
                 auto *r = state.requests.find(reqId);
// This controls which part of the program runs next, based on the current condition or result.
                 if (!r)
// This reports an error so the caller can handle the failed operation.
                   throw bl::NotFoundError("Request not found");

// This creates or updates a value that the following logic uses.
                 json respondersList = json::array();
// This controls which part of the program runs next, based on the current condition or result.
                 for (const auto &uname : r->responders()) {
// This creates or updates a value that the following logic uses.
                   auto dRef = state.donors.find(uname);
// This controls which part of the program runs next, based on the current condition or result.
                   if (dRef) {
// This creates or updates a value that the following logic uses.
                     const auto &d = dRef->get();
// This is part of the current expression or block; read it together with the nearby lines.
                     respondersList.push_back(
// This is part of the current expression or block; read it together with the nearby lines.
                         {{"username", d.username()},
// This is part of the current expression or block; read it together with the nearby lines.
                          {"fullName", d.fullName()},
// This is part of the current expression or block; read it together with the nearby lines.
                          {"bloodGroup", d.bloodGroupString()},
// This performs one step of the current operation, then continues to the next statement.
                          {"phone", d.phone()}});
// This closes the block opened above, such as a function, class, or control-flow branch.
                   } else {
// This creates or updates a value that the following logic uses.
                     auto *acc = state.accounts.findByUsername(uname);
// This is part of the current expression or block; read it together with the nearby lines.
                     respondersList.push_back(
// This is part of the current expression or block; read it together with the nearby lines.
                         {{"username", uname},
// This is part of the current expression or block; read it together with the nearby lines.
                          {"fullName", acc ? acc->fullName() : uname},
// This is part of the current expression or block; read it together with the nearby lines.
                          {"bloodGroup", "Unknown"},
// This performs one step of the current operation, then continues to the next statement.
                          {"phone", "N/A"}});
// This closes the block opened above, such as a function, class, or control-flow branch.
                   }
// This closes the block opened above, such as a function, class, or control-flow branch.
                 }

// This creates or updates a value that the following logic uses.
                 res.set_content(json{{"requestId", reqId},
// This is part of the current expression or block; read it together with the nearby lines.
                                      {"bloodGroup", r->bloodGroupString()},
// This is part of the current expression or block; read it together with the nearby lines.
                                      {"unitsNeeded", r->unitsNeeded()},
// This is part of the current expression or block; read it together with the nearby lines.
                                      {"responders", respondersList}}
// This is part of the current expression or block; read it together with the nearby lines.
                                     .dump(),
// This creates or updates a value that the following logic uses.
                                 "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               }
// This closes the block opened above, such as a function, class, or control-flow branch.
             });

// This is a source comment that explains nearby code.
  // ── GET /api/org/reminders ── (Donors eligible in next 14 days)
// This creates or updates a value that the following logic uses.
  server.Get("/api/org/reminders", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                       httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      requireRole(req, state, "ngo");
// This creates or updates a value that the following logic uses.
      std::string today = bl::DateUtil::today();
// This creates or updates a value that the following logic uses.
      std::string in14Days = bl::DateUtil::addDays(today, 14);

// This creates or updates a value that the following logic uses.
      std::map<std::string, json> grouped;
// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &bg :
// This is part of the current expression or block; read it together with the nearby lines.
           {bl::BloodGroup::APos, bl::BloodGroup::ANeg, bl::BloodGroup::BPos,
// This is part of the current expression or block; read it together with the nearby lines.
            bl::BloodGroup::BNeg, bl::BloodGroup::ABPos, bl::BloodGroup::ABNeg,
// This opens a scope for the related statements that follow.
            bl::BloodGroup::OPos, bl::BloodGroup::ONeg}) {
// This creates or updates a value that the following logic uses.
        grouped[bl::bloodGroupToString(bg)] = json::array();
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &d : state.donors.all()) {
// This is a source comment that explains nearby code.
        // Privacy filter: consented donors only!
// This controls which part of the program runs next, based on the current condition or result.
        if (!d.hasConsent() || !d.bloodGroup())
// This changes the current loop flow: stop this loop, or move to its next iteration.
          continue;

// This creates or updates a value that the following logic uses.
        std::string nextEl = d.nextEligibleDate();
// This controls which part of the program runs next, based on the current condition or result.
        if (nextEl.empty() || (nextEl >= today && nextEl <= in14Days)) {
// This is part of the current expression or block; read it together with the nearby lines.
          grouped[d.bloodGroupString()].push_back(
// This is part of the current expression or block; read it together with the nearby lines.
              {{"username", d.username()},
// This is part of the current expression or block; read it together with the nearby lines.
               {"fullName", d.fullName()},
// This is part of the current expression or block; read it together with the nearby lines.
               {"bloodGroup", d.bloodGroupString()},
// This is part of the current expression or block; read it together with the nearby lines.
               {"phone", d.maskedPhone()}, // Masked phone for privacy
// This is part of the current expression or block; read it together with the nearby lines.
               {"nextEligibleDate", nextEl.empty() ? "Eligible Now" : nextEl},
// This performs one step of the current operation, then continues to the next statement.
               {"daysRemaining", d.daysUntilEligible(today)}});
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      res.set_content(json(grouped).dump(), "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── GET /api/org/donors ── (Consented donors search with filters &
// This is a source comment that explains nearby code.
  // pagination)
// This creates or updates a value that the following logic uses.
  server.Get("/api/org/donors", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                    httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      requireRole(req, state, "ngo");

// This creates or updates a value that the following logic uses.
      std::string query = req.has_param("q") ? req.get_param_value("q") : "";
// This creates or updates a value that the following logic uses.
      std::string bgFilter =
// This performs one step of the current operation, then continues to the next statement.
          req.has_param("bg") ? req.get_param_value("bg") : "";
// This creates or updates a value that the following logic uses.
      bool eligibleOnly = req.has_param("eligibleOnly") &&
// This performs one step of the current operation, then continues to the next statement.
                          req.get_param_value("eligibleOnly") == "true";
// This creates or updates a value that the following logic uses.
      int page =
// This performs one step of the current operation, then continues to the next statement.
          req.has_param("page") ? std::stoi(req.get_param_value("page")) : 1;

// This performs one step of the current operation, then continues to the next statement.
      std::transform(query.begin(), query.end(), query.begin(), ::tolower);

// This creates or updates a value that the following logic uses.
      std::vector<json> filtered;
// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &d : state.donors.all()) {
// This is a source comment that explains nearby code.
        // Privacy filter: only consented donors appear to organizations!
// This controls which part of the program runs next, based on the current condition or result.
        if (!d.hasConsent())
// This changes the current loop flow: stop this loop, or move to its next iteration.
          continue;

// This controls which part of the program runs next, based on the current condition or result.
        if (!bgFilter.empty() && d.bloodGroupString() != bgFilter)
// This changes the current loop flow: stop this loop, or move to its next iteration.
          continue;
// This controls which part of the program runs next, based on the current condition or result.
        if (eligibleOnly && !d.isEligibleOnDate())
// This changes the current loop flow: stop this loop, or move to its next iteration.
          continue;

// This controls which part of the program runs next, based on the current condition or result.
        if (!query.empty()) {
// This creates or updates a value that the following logic uses.
          std::string name = d.fullName();
// This creates or updates a value that the following logic uses.
          std::string bg = d.bloodGroupString();
// This performs one step of the current operation, then continues to the next statement.
          std::transform(name.begin(), name.end(), name.begin(), ::tolower);
// This performs one step of the current operation, then continues to the next statement.
          std::transform(bg.begin(), bg.end(), bg.begin(), ::tolower);
// This controls which part of the program runs next, based on the current condition or result.
          if (name.find(query) == std::string::npos &&
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
              bg.find(query) == std::string::npos) {
// This changes the current loop flow: stop this loop, or move to its next iteration.
            continue;
// This closes the block opened above, such as a function, class, or control-flow branch.
          }
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This is part of the current expression or block; read it together with the nearby lines.
        filtered.push_back(d.profileToJson(true)); // Phone is masked!
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      auto paginated = bl::paginate(filtered, page, 10);
// This is part of the current expression or block; read it together with the nearby lines.
      res.set_content(
// This creates or updates a value that the following logic uses.
          paginated.toJson([](const json &item) { return item; }).dump(),
// This creates or updates a value that the following logic uses.
          "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── GET /api/org/profile & PUT /api/org/profile ──
// This is part of the current expression or block; read it together with the nearby lines.
  server.Get("/api/org/profile",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
             [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
               try {
// This creates or updates a value that the following logic uses.
                 auto *account = requireRole(req, state, "ngo");
// This creates or updates a value that the following logic uses.
                 res.set_content(account->toJson().dump(), "application/json");
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               }
// This closes the block opened above, such as a function, class, or control-flow branch.
             });

// This creates or updates a value that the following logic uses.
  server.Put("/api/org/profile", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                     httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      checkPayloadSize(req);
// This creates or updates a value that the following logic uses.
      auto *account = requireRole(req, state, "ngo");
// This creates or updates a value that the following logic uses.
      auto *org = dynamic_cast<bl::Organization *>(account);
// This controls which part of the program runs next, based on the current condition or result.
      if (!org)
// This reports an error so the caller can handle the failed operation.
        throw bl::BadRequestError("Not an organization account");

// This creates or updates a value that the following logic uses.
      auto body = json::parse(req.body);
// This controls which part of the program runs next, based on the current condition or result.
      if (body.contains("fullName"))
// This creates or updates a value that the following logic uses.
        org->setFullName(body["fullName"].get<std::string>());
// This controls which part of the program runs next, based on the current condition or result.
      if (body.contains("city"))
// This creates or updates a value that the following logic uses.
        org->setCity(body["city"].get<std::string>());
// This controls which part of the program runs next, based on the current condition or result.
      if (body.contains("contactPhone"))
// This creates or updates a value that the following logic uses.
        org->setContactPhone(body["contactPhone"].get<std::string>());
// This controls which part of the program runs next, based on the current condition or result.
      if (body.contains("contactEmail"))
// This creates or updates a value that the following logic uses.
        org->setContactEmail(body["contactEmail"].get<std::string>());

// This performs one step of the current operation, then continues to the next statement.
      state.accounts.save();
// This is part of the current expression or block; read it together with the nearby lines.
      state.audit.log(account->username(), "ORG_PROFILE_UPDATE",
// This performs one step of the current operation, then continues to the next statement.
                      "Updated organization info");

// This creates or updates a value that the following logic uses.
      res.set_content(org->toJson().dump(), "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── CSV Export 1: /api/org/export/donors.csv ── (Consented donors only,
// This is a source comment that explains nearby code.
  // phones masked)
// This creates or updates a value that the following logic uses.
  server.Get("/api/org/export/donors.csv", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                               httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      requireRole(req, state, "ngo");
// This performs one step of the current operation, then continues to the next statement.
      std::stringstream ss;
// This is part of the current expression or block; read it together with the nearby lines.
      ss << "Username,Full Name,Blood Group,Age,Weight,Phone,Last "
// This performs one step of the current operation, then continues to the next statement.
            "Donation,Next Eligible Date\n";
// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &d : state.donors.all()) {
// This controls which part of the program runs next, based on the current condition or result.
        if (!d.hasConsent())
// This changes the current loop flow: stop this loop, or move to its next iteration.
          continue; // Consent filter
// This is part of the current expression or block; read it together with the nearby lines.
        ss << d.username() << "," << d.fullName() << "," << d.bloodGroupString()
// This is part of the current expression or block; read it together with the nearby lines.
           << "," << d.age() << "," << d.weight() << "," << d.maskedPhone()
// This is part of the current expression or block; read it together with the nearby lines.
           << "," // Security: masked phone
// This performs one step of the current operation, then continues to the next statement.
           << d.lastDonationDate() << "," << d.nextEligibleDate() << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This performs one step of the current operation, then continues to the next statement.
      res.set_content(ss.str(), "text/csv");
// This is part of the current expression or block; read it together with the nearby lines.
      res.set_header("Content-Disposition",
// This performs one step of the current operation, then continues to the next statement.
                     "attachment; filename=\"consented_donors.csv\"");
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── CSV Export 2: /api/org/export/camps.csv ──
// This is part of the current expression or block; read it together with the nearby lines.
  server.Get("/api/org/export/camps.csv",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
             [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
               try {
// This performs one step of the current operation, then continues to the next statement.
                 requireRole(req, state, "ngo");
// This performs one step of the current operation, then continues to the next statement.
                 std::stringstream ss;
// This is part of the current expression or block; read it together with the nearby lines.
                 ss << "Camp ID,Camp Name,Date,Target Units,Units "
// This performs one step of the current operation, then continues to the next statement.
                       "Collected,Deferred Count,Status\n";
// This controls which part of the program runs next, based on the current condition or result.
                 for (const auto &c : state.camps.all()) {
// This is part of the current expression or block; read it together with the nearby lines.
                   ss << c.id() << "," << c.name() << "," << c.date() << ","
// This is part of the current expression or block; read it together with the nearby lines.
                      << c.targetUnits() << "," << c.unitsCollected() << ","
// This performs one step of the current operation, then continues to the next statement.
                      << c.deferredCount() << "," << c.statusString() << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
                 }
// This performs one step of the current operation, then continues to the next statement.
                 res.set_content(ss.str(), "text/csv");
// This is part of the current expression or block; read it together with the nearby lines.
                 res.set_header("Content-Disposition",
// This performs one step of the current operation, then continues to the next statement.
                                "attachment; filename=\"camp_results.csv\"");
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
                 handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
               }
// This closes the block opened above, such as a function, class, or control-flow branch.
             });

// This is a source comment that explains nearby code.
  // ── GET /api/admin/orgs ── (Admin approval board)
// This creates or updates a value that the following logic uses.
  server.Get("/api/admin/orgs", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                    httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      requireAdmin(req, state);
// This creates or updates a value that the following logic uses.
      json orgs = json::array();
// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &acc : state.accounts.all()) {
// This controls which part of the program runs next, based on the current condition or result.
        if (auto *org = dynamic_cast<bl::Organization *>(acc.get())) {
// This performs one step of the current operation, then continues to the next statement.
          orgs.push_back(org->toJson());
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This creates or updates a value that the following logic uses.
      res.set_content(orgs.dump(), "application/json");
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── PUT /api/admin/orgs/:username/status ── (Approve/Reject org)
// This is part of the current expression or block; read it together with the nearby lines.
  server.Put(
// This is part of the current expression or block; read it together with the nearby lines.
      R"(/api/admin/orgs/([^/]+)/status)",
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
      [&](const httplib::Request &req, httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
        try {
// This performs one step of the current operation, then continues to the next statement.
          checkPayloadSize(req);
// This creates or updates a value that the following logic uses.
          auto *admin = requireAdmin(req, state);
// This creates or updates a value that the following logic uses.
          std::string orgUser = req.matches[1];

// This creates or updates a value that the following logic uses.
          auto *acc = state.accounts.findByUsername(orgUser);
// This controls which part of the program runs next, based on the current condition or result.
          if (!acc)
// This reports an error so the caller can handle the failed operation.
            throw bl::NotFoundError("Organization account not found");
// This creates or updates a value that the following logic uses.
          auto *org = dynamic_cast<bl::Organization *>(acc);
// This controls which part of the program runs next, based on the current condition or result.
          if (!org)
// This reports an error so the caller can handle the failed operation.
            throw bl::BadRequestError("Account is not an organization");

// This creates or updates a value that the following logic uses.
          auto body = json::parse(req.body);
// This creates or updates a value that the following logic uses.
          std::string newStatus = body.at("status").get<std::string>();
// This controls which part of the program runs next, based on the current condition or result.
          if (newStatus != "Approved" && newStatus != "Rejected") {
// This reports an error so the caller can handle the failed operation.
            throw bl::BadRequestError("Status must be Approved or Rejected");
// This closes the block opened above, such as a function, class, or control-flow branch.
          }

// This performs one step of the current operation, then continues to the next statement.
          org->setApprovalStatus(newStatus);
// This performs one step of the current operation, then continues to the next statement.
          state.accounts.save();

// This is part of the current expression or block; read it together with the nearby lines.
          state.audit.log(admin->username(), "ORG_APPROVAL",
// This performs one step of the current operation, then continues to the next statement.
                          "Set " + orgUser + " status to " + newStatus);
// This is part of the current expression or block; read it together with the nearby lines.
          state.notifications.send(
// This is part of the current expression or block; read it together with the nearby lines.
              orgUser, "Account Status Updated",
// This performs one step of the current operation, then continues to the next statement.
              "Your BloodLine organization account has been " + newStatus);

// This creates or updates a value that the following logic uses.
          res.set_content(org->toJson().dump(), "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
        } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
          handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
        } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
          handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
      });

// This is a source comment that explains nearby code.
  // ── GET /api/admin/audit-log ── (Admin view with pagination)
// This creates or updates a value that the following logic uses.
  server.Get("/api/admin/audit-log", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                         httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This performs one step of the current operation, then continues to the next statement.
      requireAdmin(req, state);
// This creates or updates a value that the following logic uses.
      int page =
// This performs one step of the current operation, then continues to the next statement.
          req.has_param("page") ? std::stoi(req.get_param_value("page")) : 1;
// This creates or updates a value that the following logic uses.
      auto entries = state.audit.all();
// This is part of the current expression or block; read it together with the nearby lines.
      std::reverse(entries.begin(), entries.end()); // newest first

// This creates or updates a value that the following logic uses.
      auto paginated = bl::paginate(entries, page, 10);
// This is part of the current expression or block; read it together with the nearby lines.
      res.set_content(
// This creates or updates a value that the following logic uses.
          paginated.toJson([](const bl::AuditEntry &e) { return e.toJson(); })
// This is part of the current expression or block; read it together with the nearby lines.
              .dump(),
// This creates or updates a value that the following logic uses.
          "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const bl::AppError &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── GET /api/public/records ── (Consented donors only; with pagination)
// This creates or updates a value that the following logic uses.
  server.Get("/api/public/records", [&](const httplib::Request &req,
// This opens a scope for the related statements that follow.
                                        httplib::Response &res) {
// This controls which part of the program runs next, based on the current condition or result.
    try {
// This creates or updates a value that the following logic uses.
      std::string query = req.has_param("q") ? req.get_param_value("q") : "";
// This creates or updates a value that the following logic uses.
      int page =
// This performs one step of the current operation, then continues to the next statement.
          req.has_param("page") ? std::stoi(req.get_param_value("page")) : 1;
// This performs one step of the current operation, then continues to the next statement.
      std::transform(query.begin(), query.end(), query.begin(), ::tolower);

// This creates or updates a value that the following logic uses.
      std::vector<json> filtered;
// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &d : state.donations.all()) {
// This is a source comment that explains nearby code.
        // Privacy filter: skip non-consented donors completely
// This controls which part of the program runs next, based on the current condition or result.
        if (!donorHasConsent(state, d.donorUsername()))
// This changes the current loop flow: stop this loop, or move to its next iteration.
          continue;

// This controls which part of the program runs next, based on the current condition or result.
        if (!query.empty()) {
// This creates or updates a value that the following logic uses.
          std::string name = d.donorName();
// This creates or updates a value that the following logic uses.
          std::string bg = d.bloodGroup();
// This creates or updates a value that the following logic uses.
          std::string camp = d.campName();
// This performs one step of the current operation, then continues to the next statement.
          std::transform(name.begin(), name.end(), name.begin(), ::tolower);
// This performs one step of the current operation, then continues to the next statement.
          std::transform(bg.begin(), bg.end(), bg.begin(), ::tolower);
// This performs one step of the current operation, then continues to the next statement.
          std::transform(camp.begin(), camp.end(), camp.begin(), ::tolower);
// This controls which part of the program runs next, based on the current condition or result.
          if (name.find(query) == std::string::npos &&
// This creates or updates a value that the following logic uses.
              bg.find(query) == std::string::npos &&
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
              camp.find(query) == std::string::npos) {
// This changes the current loop flow: stop this loop, or move to its next iteration.
            continue;
// This closes the block opened above, such as a function, class, or control-flow branch.
          }
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        filtered.push_back(d.toJson());
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This creates or updates a value that the following logic uses.
      auto paginated = bl::paginate(filtered, page, 10);
// This is part of the current expression or block; read it together with the nearby lines.
      res.set_content(
// This creates or updates a value that the following logic uses.
          paginated.toJson([](const json &item) { return item; }).dump(),
// This creates or updates a value that the following logic uses.
          "application/json");

// This closes the block opened above, such as a function, class, or control-flow branch.
    } catch (const std::exception &e) {
// This performs one step of the current operation, then continues to the next statement.
      handleError(res, e);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  });

// This is a source comment that explains nearby code.
  // ── GET /api/compatibility ──
// This creates or updates a value that the following logic uses.
  server.Get("/api/compatibility", [&](const httplib::Request &,
// This opens a scope for the related statements that follow.
                                       httplib::Response &res) {
// This creates or updates a value that the following logic uses.
    res.set_content(state.compatibility.toJson().dump(), "application/json");
// This closes the block opened above, such as a function, class, or control-flow branch.
  });
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Main: Server initialization and file mounting
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
int main(int argc, char *argv[]) {
// This creates or updates a value that the following logic uses.
  std::string dataDir = "data";
// This creates or updates a value that the following logic uses.
  std::string frontendDir = "frontend";
// This creates or updates a value that the following logic uses.
  int port = 8080;

// This controls which part of the program runs next, based on the current condition or result.
  if (argc > 1)
// This performs one step of the current operation, then continues to the next statement.
    dataDir = argv[1];
// This controls which part of the program runs next, based on the current condition or result.
  if (argc > 2)
// This performs one step of the current operation, then continues to the next statement.
    frontendDir = argv[2];
// This controls which part of the program runs next, based on the current condition or result.
  if (argc > 3)
// This performs one step of the current operation, then continues to the next statement.
    port = std::stoi(argv[3]);

// This is part of the current expression or block; read it together with the nearby lines.
  std::cout << "╔══════════════════════════════════════════╗\n"
// This is part of the current expression or block; read it together with the nearby lines.
            << "║       BloodLine Server v2.0 (Phase 2)    ║\n"
// This performs one step of the current operation, then continues to the next statement.
            << "╚══════════════════════════════════════════╝\n";

// This performs one step of the current operation, then continues to the next statement.
  AppState state(dataDir);
// This performs one step of the current operation, then continues to the next statement.
  state.loadAll();
// This performs one step of the current operation, then continues to the next statement.
  seedData(state);

// This performs one step of the current operation, then continues to the next statement.
  httplib::Server server;
// This performs one step of the current operation, then continues to the next statement.
  server.set_mount_point("/", frontendDir);

// This performs one step of the current operation, then continues to the next statement.
  setupRoutes(server, state);

// This is part of the current expression or block; read it together with the nearby lines.
  std::cout << "[server] Serving data from:     " << dataDir << "\n"
// This is part of the current expression or block; read it together with the nearby lines.
            << "[server] Serving frontend from: " << frontendDir << "\n"
// This is part of the current expression or block; read it together with the nearby lines.
            << "[server] BloodLine API ready at http://localhost:" << port
// This performs one step of the current operation, then continues to the next statement.
            << "\n";

// This performs one step of the current operation, then continues to the next statement.
  server.listen("0.0.0.0", port);
// This sends a result back to the code that called this function.
  return 0;
// This closes the block opened above, such as a function, class, or control-flow branch.
}
