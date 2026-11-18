#pragma once
#include "bloodline/IRepository.hpp"
#include "bloodline/Donor.hpp"
#include "bloodline/Camp.hpp"
#include "bloodline/Donation.hpp"
#include "bloodline/Account.hpp"
#include "bloodline/Organization.hpp"
#include "bloodline/Credential.hpp"
#include "bloodline/Request.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <cmath>

namespace bloodline {

inline std::vector<std::string> splitCsv(const std::string& line, char delim = ',') {
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                token += '"'; ++i;
            } else inQuotes = !inQuotes;
        } else if (c == delim && !inQuotes) {
            tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    tokens.push_back(token);
    return tokens;
}

inline std::string csvField(const std::string& value) {
    std::string result = "\"";
    for (char c : value) { if (c == '"') result += '"'; result += c; }
    return result + "\"";
}

inline void atomicWrite(const std::string& path, const std::string& content) {
    const std::string temporary = path + ".tmp";
    std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
    output.exceptions(std::ios::badbit | std::ios::failbit);
    output << content;
    output.flush();
    output.close();
    std::filesystem::rename(temporary, path);
}

// ──────────────────────────────────────────────
// Templates: Generic filter and pagination helpers
// ──────────────────────────────────────────────

template <typename T>
struct PageResult {
    std::vector<T> items;
    int totalItems = 0;
    int currentPage = 1;
    int pageSize = 10;
    int totalPages = 1;

    nlohmann::json toJson(const std::function<nlohmann::json(const T&)>& itemSerializer) const {
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& it : items) {
            arr.push_back(itemSerializer(it));
        }
        return {
            {"items", arr},
            {"totalItems", totalItems},
            {"currentPage", currentPage},
            {"pageSize", pageSize},
            {"totalPages", totalPages}
        };
    }
};

template <typename T>
PageResult<T> paginate(const std::vector<T>& allItems, int page = 1, int pageSize = 10) {
    if (page < 1) page = 1;
    if (pageSize < 1) pageSize = 10;

    PageResult<T> res;
    res.totalItems = static_cast<int>(allItems.size());
    res.pageSize = pageSize;
    res.totalPages = res.totalItems > 0 ? static_cast<int>(std::ceil(static_cast<double>(res.totalItems) / pageSize)) : 1;
    res.currentPage = (page > res.totalPages) ? res.totalPages : page;

    int startIndex = (res.currentPage - 1) * pageSize;
    if (startIndex < res.totalItems) {
        int endIndex = std::min(startIndex + pageSize, res.totalItems);
        for (int i = startIndex; i < endIndex; ++i) {
            res.items.push_back(allItems[i]);
        }
    }
    return res;
}

// ──────────────────────────────────────────────
// DonorRepository: persists Donor profiles to CSV
// ──────────────────────────────────────────────

class DonorFileRepository final : public IRepository<Donor> {
    std::string filepath_;
    std::vector<Donor> donors_;
    mutable std::mutex mtx_;

public:
    explicit DonorFileRepository(std::string filepath)
        : filepath_(std::move(filepath)) {}

    void load() override {
        std::lock_guard lock(mtx_);
        donors_.clear();
        std::ifstream file(filepath_);
        if (!file.is_open()) return;

        std::string line;
        std::getline(file, line); // header
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            auto cols = splitCsv(line);
            if (cols.size() < 8) continue;

            Donor donor(cols[0], cols[1], Credential{});
            donor.setAge(std::stoi(cols[2]));
            donor.setGender(cols[3]);
            if (!cols[4].empty()) {
                donor.setBloodGroup(bloodGroupFromString(cols[4]));
            }
            donor.setWeight(std::stod(cols[5]));
            donor.setPhone(cols[6]);
            if (cols[7] == "true") donor.grantConsent();
            else donor.revokeConsent();
            donor.markProfileComplete();

            if (cols.size() >= 9) {
                donor.setLastDonationDate(cols[8]);
            }
            if (cols.size() >= 10 && !cols[9].empty()) {
                donor.setDonationCount(std::stoi(cols[9]));
            }

            donors_.push_back(std::move(donor));
        }
    }

    void save() override {
        std::lock_guard lock(mtx_);
        std::filesystem::create_directories(
            std::filesystem::path(filepath_).parent_path());
        std::ostringstream file;
        file << "username,fullName,age,gender,bloodGroup,weight,phone,consent,lastDonationDate,donationCount\n";
        for (const auto& d : donors_) {
            file << csvField(d.username()) << ","
                 << csvField(d.fullName()) << ","
                 << d.age() << ","
                 << csvField(d.gender()) << ","
                 << csvField(d.bloodGroupString()) << ","
                 << d.weight() << ","
                 << csvField(d.phone()) << ","
                 << (d.hasConsent() ? "true" : "false") << ","
                 << csvField(d.lastDonationDate()) << ","
                 << d.donationCount() << "\n";
        }
        atomicWrite(filepath_, file.str());
    }

    std::vector<Donor>& all() override { return donors_; }
    const std::vector<Donor>& all() const override { return donors_; }

    std::optional<std::reference_wrapper<Donor>> find(const std::string& username) override {
        for (auto& d : donors_) {
            if (d.username() == username) return d;
        }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<const Donor>> find(
        const std::string& username) const override {
        for (const auto& d : donors_) {
            if (d.username() == username) return std::cref(d);
        }
        return std::nullopt;
    }

    void add(Donor donor) override {
        std::lock_guard lock(mtx_);
        donors_.push_back(std::move(donor));
    }
};

// ──────────────────────────────────────────────
// CampRepository: persists Camps to CSV
// ──────────────────────────────────────────────

class CampFileRepository final : public IRepository<Camp> {
    std::string filepath_;
    std::vector<Camp> camps_;
    mutable std::mutex mtx_;

public:
    explicit CampFileRepository(std::string filepath)
        : filepath_(std::move(filepath)) {}

    void load() override {
        std::lock_guard lock(mtx_);
        camps_.clear();
        std::ifstream file(filepath_);
        if (!file.is_open()) return;

        std::string line;
        std::getline(file, line); // header
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            auto cols = splitCsv(line);
            if (cols.size() < 5) continue;
            int target = std::stoi(cols[3]);
            int collected = std::stoi(cols[4]);
            int slotCap = (cols.size() >= 6 && !cols[5].empty()) ? std::stoi(cols[5]) : target * 2;
            CampStatus status = (cols.size() >= 7) ? campStatusFromString(cols[6]) : CampStatus::Upcoming;
            int deferred = (cols.size() >= 8 && !cols[7].empty()) ? std::stoi(cols[7]) : 0;

            Camp camp(cols[0], cols[1], cols[2], target, slotCap, status, collected, deferred);

            // Parse registered donors list (semicolon separated in col 8)
            if (cols.size() >= 9 && !cols[8].empty()) {
                auto reg = splitCsv(cols[8], ';');
                for (const auto& r : reg) {
                    if (!r.empty()) camp.addRegisteredDonorDirect(r);
                }
            }

            camps_.push_back(std::move(camp));
        }
    }

    void save() override {
        std::lock_guard lock(mtx_);
        std::filesystem::create_directories(
            std::filesystem::path(filepath_).parent_path());
        std::ostringstream file;
        file << "id,name,date,targetUnits,unitsCollected,slotCapacity,status,deferredCount,registeredDonors\n";
        for (const auto& c : camps_) {
            std::string regStr;
            for (size_t i = 0; i < c.registeredDonors().size(); ++i) {
                if (i > 0) regStr += ";";
                regStr += c.registeredDonors()[i];
            }

            file << csvField(c.id()) << ","
                 << csvField(c.name()) << ","
                 << csvField(c.date()) << ","
                 << c.targetUnits() << ","
                 << c.unitsCollected() << ","
                 << c.slotCapacity() << ","
                 << csvField(c.statusString()) << ","
                 << c.deferredCount() << ","
                 << csvField(regStr) << "\n";
        }
        atomicWrite(filepath_, file.str());
    }

    std::vector<Camp>& all() override { return camps_; }
    const std::vector<Camp>& all() const override { return camps_; }

    std::optional<std::reference_wrapper<Camp>> find(const std::string& id) override {
        for (auto& c : camps_) {
            if (c.id() == id) return c;
        }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<const Camp>> find(
        const std::string& id) const override {
        for (const auto& c : camps_) {
            if (c.id() == id) return std::cref(c);
        }
        return std::nullopt;
    }

    void add(Camp camp) override {
        std::lock_guard lock(mtx_);
        camps_.push_back(std::move(camp));
    }
};

// ──────────────────────────────────────────────
// DonationRepository: persists Donations to CSV
// ──────────────────────────────────────────────

class DonationFileRepository final : public IRepository<Donation> {
    std::string filepath_;
    std::vector<Donation> donations_;
    mutable std::mutex mtx_;

public:
    explicit DonationFileRepository(std::string filepath)
        : filepath_(std::move(filepath)) {}

    void load() override {
        std::lock_guard lock(mtx_);
        donations_.clear();
        std::ifstream file(filepath_);
        if (!file.is_open()) return;

        std::string line;
        std::getline(file, line); // header
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            auto cols = splitCsv(line);
            if (cols.size() < 8) continue;
            donations_.emplace_back(
                cols[0], cols[1], cols[2], cols[3],
                cols[4], cols[5], cols[6], cols[7]);
        }
    }

    void save() override {
        std::lock_guard lock(mtx_);
        std::filesystem::create_directories(
            std::filesystem::path(filepath_).parent_path());
        std::ostringstream file;
        file << "id,donorUsername,donorName,bloodGroup,campId,campName,date,status\n";
        for (const auto& d : donations_) {
            file << csvField(d.id()) << ","
                 << csvField(d.donorUsername()) << ","
                 << csvField(d.donorName()) << ","
                 << csvField(d.bloodGroup()) << ","
                 << csvField(d.campId()) << ","
                 << csvField(d.campName()) << ","
                 << csvField(d.date()) << ","
                 << csvField(d.status()) << "\n";
        }
        atomicWrite(filepath_, file.str());
    }

    std::vector<Donation>& all() override { return donations_; }
    const std::vector<Donation>& all() const override { return donations_; }

    std::optional<std::reference_wrapper<Donation>> find(
        const std::string& id) override {
        for (auto& d : donations_) {
            if (d.id() == id) return d;
        }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<const Donation>> find(
        const std::string& id) const override {
        for (const auto& d : donations_) {
            if (d.id() == id) return std::cref(d);
        }
        return std::nullopt;
    }

    void add(Donation donation) override {
        std::lock_guard lock(mtx_);
        donations_.push_back(std::move(donation));
    }
};

// ──────────────────────────────────────────────
// AccountRepository: persists Account credentials
// ──────────────────────────────────────────────

class AccountFileRepository {
    std::string filepath_;
    std::vector<std::unique_ptr<Account>> accounts_;
    mutable std::mutex mtx_;

public:
    explicit AccountFileRepository(std::string filepath)
        : filepath_(std::move(filepath)) {}

    void load() {
        std::lock_guard lock(mtx_);
        accounts_.clear();
        std::ifstream file(filepath_);
        if (!file.is_open()) return;

        std::string line;
        std::getline(file, line); // header
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            auto cols = splitCsv(line);
            if (cols.size() < 5) continue;
            // username,fullName,role,salt,hash,orgType,approvalStatus,city
            auto cred = Credential::fromStored(cols[3], cols[4]);
            std::string role = cols[2];

            if (role == "donor") {
                accounts_.push_back(
                    std::make_unique<Donor>(cols[0], cols[1], std::move(cred)));
            } else if (role == "admin") {
                accounts_.push_back(
                    std::make_unique<Admin>(cols[0], cols[1], std::move(cred)));
            } else {
                std::string orgType = (cols.size() >= 6 && !cols[5].empty()) ? cols[5] : "ngo";
                std::string approval = (cols.size() >= 7 && !cols[6].empty()) ? cols[6] : "Approved";
                std::string city = (cols.size() >= 8 && !cols[7].empty()) ? cols[7] : "Nainital";
                std::string phone = (cols.size() >= 9 && !cols[8].empty()) ? cols[8] : "05942-235022";
                std::string email = (cols.size() >= 10 && !cols[9].empty()) ? cols[9] : "bloodbank@ramsayhospital.org";

                if (orgType == "hospital") {
                    accounts_.push_back(
                        std::make_unique<Hospital>(cols[0], cols[1], std::move(cred), approval, city, phone, email));
                } else {
                    accounts_.push_back(
                        std::make_unique<NGO>(cols[0], cols[1], std::move(cred), approval, city, phone, email));
                }
            }
        }
    }

    void save() {
        std::lock_guard lock(mtx_);
        std::filesystem::create_directories(
            std::filesystem::path(filepath_).parent_path());
        std::ostringstream file;
        file << "username,fullName,role,salt,hash,orgType,approvalStatus,city,phone,email\n";
        for (const auto& acc : accounts_) {
            std::string orgType = "";
            std::string approval = "Approved";
            std::string city = "Nainital";
            std::string phone = "";
            std::string email = "";

            auto* org = dynamic_cast<Organization*>(acc.get());
            if (org) {
                approval = org->approvalStatus();
                city = org->city();
                phone = org->contactPhone();
                email = org->contactEmail();
                orgType = (dynamic_cast<Hospital*>(org) != nullptr) ? "hospital" : "ngo";
            }

            file << csvField(acc->username()) << ","
                 << csvField(acc->fullName()) << ","
                 << csvField(acc->role()) << ","
                 << acc->credential().salt() << ","
                 << acc->credential().hash() << ","
                 << csvField(orgType) << ","
                 << csvField(approval) << ","
                 << csvField(city) << ","
                 << csvField(phone) << ","
                 << csvField(email) << "\n";
        }
        atomicWrite(filepath_, file.str());
    }

    std::vector<std::unique_ptr<Account>>& all() { return accounts_; }

    Account* findByUsername(const std::string& username) {
        for (auto& acc : accounts_) {
            if (acc->username() == username) return acc.get();
        }
        return nullptr;
    }

    const Account* findByUsername(const std::string& username) const {
        for (const auto& acc : accounts_) {
            if (acc->username() == username) return acc.get();
        }
        return nullptr;
    }

    void add(std::unique_ptr<Account> account) {
        std::lock_guard lock(mtx_);
        accounts_.push_back(std::move(account));
    }
};

// ──────────────────────────────────────────────
// RequestFileRepository: persists Blood Requests
// ──────────────────────────────────────────────

class RequestFileRepository {
    std::string filepath_;
    std::vector<std::unique_ptr<Request>> requests_;
    mutable std::mutex mtx_;

public:
    explicit RequestFileRepository(std::string filepath)
        : filepath_(std::move(filepath)) {}

    void load() {
        std::lock_guard lock(mtx_);
        requests_.clear();
        std::ifstream file(filepath_);
        if (!file.is_open()) return;

        std::string line;
        std::getline(file, line); // header
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            auto cols = splitCsv(line);
            if (cols.size() < 8) continue;
            // id,orgUsername,hospitalName,bloodGroup,unitsNeeded,status,urgency,createdAt,responders
            BloodGroup bg = bloodGroupFromString(cols[3]);
            int needed = std::stoi(cols[4]);
            RequestStatus st = requestStatusFromString(cols[5]);
            std::string urgency = cols[6];
            std::string createdAt = cols[7];

            std::unique_ptr<Request> req;
            if (urgency == "Critical") {
                req = std::make_unique<UrgentRequest>(cols[0], cols[1], cols[2], bg, needed, true, createdAt, st);
            } else if (urgency == "Urgent") {
                req = std::make_unique<UrgentRequest>(cols[0], cols[1], cols[2], bg, needed, false, createdAt, st);
            } else {
                req = std::make_unique<RoutineRequest>(cols[0], cols[1], cols[2], bg, needed, createdAt, st);
            }

            if (cols.size() >= 9 && !cols[8].empty()) {
                auto resps = splitCsv(cols[8], ';');
                for (const auto& r : resps) {
                    if (!r.empty()) req->restoreResponder(r);
                }
            }

            requests_.push_back(std::move(req));
        }
    }

    void save() {
        std::lock_guard lock(mtx_);
        std::filesystem::create_directories(
            std::filesystem::path(filepath_).parent_path());
        std::ostringstream file;
        file << "id,orgUsername,hospitalName,bloodGroup,unitsNeeded,status,urgency,createdAt,responders\n";
        for (const auto& r : requests_) {
            std::string respStr;
            for (size_t i = 0; i < r->responders().size(); ++i) {
                if (i > 0) respStr += ";";
                respStr += r->responders()[i];
            }
            file << csvField(r->id()) << ","
                 << csvField(r->orgUsername()) << ","
                 << csvField(r->hospitalName()) << ","
                 << csvField(r->bloodGroupString()) << ","
                 << r->unitsNeeded() << ","
                 << csvField(r->statusString()) << ","
                 << csvField(r->urgency()) << ","
                 << csvField(r->createdAt()) << ","
                 << csvField(respStr) << "\n";
        }
        atomicWrite(filepath_, file.str());
    }

    std::vector<std::unique_ptr<Request>>& all() { return requests_; }

    Request* find(const std::string& id) {
        for (auto& r : requests_) {
            if (r->id() == id) return r.get();
        }
        return nullptr;
    }

    void add(std::unique_ptr<Request> req) {
        std::lock_guard lock(mtx_);
        requests_.push_back(std::move(req));
    }
};

} // namespace bloodline
