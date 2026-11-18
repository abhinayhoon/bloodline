// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/FileRepository.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in "bloodline/IRepository.hpp", so this file can use its declarations.
#include "bloodline/IRepository.hpp"
// This brings in "bloodline/Donor.hpp", so this file can use its declarations.
#include "bloodline/Donor.hpp"
// This brings in "bloodline/Camp.hpp", so this file can use its declarations.
#include "bloodline/Camp.hpp"
// This brings in "bloodline/Donation.hpp", so this file can use its declarations.
#include "bloodline/Donation.hpp"
// This brings in "bloodline/Account.hpp", so this file can use its declarations.
#include "bloodline/Account.hpp"
// This brings in "bloodline/Organization.hpp", so this file can use its declarations.
#include "bloodline/Organization.hpp"
// This brings in "bloodline/Credential.hpp", so this file can use its declarations.
#include "bloodline/Credential.hpp"
// This brings in "bloodline/Request.hpp", so this file can use its declarations.
#include "bloodline/Request.hpp"
// This brings in <fstream>, so this file can use its declarations.
#include <fstream>
// This brings in <sstream>, so this file can use its declarations.
#include <sstream>
// This brings in <filesystem>, so this file can use its declarations.
#include <filesystem>
// This brings in <mutex>, so this file can use its declarations.
#include <mutex>
// This brings in <algorithm>, so this file can use its declarations.
#include <algorithm>
// This brings in <iostream>, so this file can use its declarations.
#include <iostream>
// This brings in <cmath>, so this file can use its declarations.
#include <cmath>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
inline std::vector<std::string> splitCsv(const std::string& line, char delim = ',') {
// This creates or updates a value that the following logic uses.
    std::vector<std::string> tokens;
// This creates or updates a value that the following logic uses.
    std::string token;
// This creates or updates a value that the following logic uses.
    bool inQuotes = false;
// This controls which part of the program runs next, based on the current condition or result.
    for (size_t i = 0; i < line.size(); ++i) {
// This performs one step of the current operation, then continues to the next statement.
        char c = line[i];
// This controls which part of the program runs next, based on the current condition or result.
        if (c == '"') {
// This controls which part of the program runs next, based on the current condition or result.
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
// This performs one step of the current operation, then continues to the next statement.
                token += '"'; ++i;
// This closes the block opened above, such as a function, class, or control-flow branch.
            } else inQuotes = !inQuotes;
// This closes the block opened above, such as a function, class, or control-flow branch.
        } else if (c == delim && !inQuotes) {
// This performs one step of the current operation, then continues to the next statement.
            tokens.push_back(token);
// This performs one step of the current operation, then continues to the next statement.
            token.clear();
// This closes the block opened above, such as a function, class, or control-flow branch.
        } else {
// This performs one step of the current operation, then continues to the next statement.
            token += c;
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This performs one step of the current operation, then continues to the next statement.
    tokens.push_back(token);
// This sends a result back to the code that called this function.
    return tokens;
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
inline std::string csvField(const std::string& value) {
// This creates or updates a value that the following logic uses.
    std::string result = "\"";
// This controls which part of the program runs next, based on the current condition or result.
    for (char c : value) { if (c == '"') result += '"'; result += c; }
// This sends a result back to the code that called this function.
    return result + "\"";
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
inline void atomicWrite(const std::string& path, const std::string& content) {
// This creates or updates a value that the following logic uses.
    const std::string temporary = path + ".tmp";
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
// This performs one step of the current operation, then continues to the next statement.
    output.exceptions(std::ios::badbit | std::ios::failbit);
// This performs one step of the current operation, then continues to the next statement.
    output << content;
// This performs one step of the current operation, then continues to the next statement.
    output.flush();
// This performs one step of the current operation, then continues to the next statement.
    output.close();
// This performs one step of the current operation, then continues to the next statement.
    std::filesystem::rename(temporary, path);
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ──────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Templates: Generic filter and pagination helpers
// This is a source comment that explains nearby code.
// ──────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
template <typename T>
// This begins a reusable type or template: a blueprint for related data and behavior.
struct PageResult {
// This creates or updates a value that the following logic uses.
    std::vector<T> items;
// This creates or updates a value that the following logic uses.
    int totalItems = 0;
// This creates or updates a value that the following logic uses.
    int currentPage = 1;
// This creates or updates a value that the following logic uses.
    int pageSize = 10;
// This creates or updates a value that the following logic uses.
    int totalPages = 1;

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    nlohmann::json toJson(const std::function<nlohmann::json(const T&)>& itemSerializer) const {
// This creates or updates a value that the following logic uses.
        nlohmann::json arr = nlohmann::json::array();
// This controls which part of the program runs next, based on the current condition or result.
        for (const auto& it : items) {
// This performs one step of the current operation, then continues to the next statement.
            arr.push_back(itemSerializer(it));
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return {
// This is part of the current expression or block; read it together with the nearby lines.
            {"items", arr},
// This is part of the current expression or block; read it together with the nearby lines.
            {"totalItems", totalItems},
// This is part of the current expression or block; read it together with the nearby lines.
            {"currentPage", currentPage},
// This is part of the current expression or block; read it together with the nearby lines.
            {"pageSize", pageSize},
// This is part of the current expression or block; read it together with the nearby lines.
            {"totalPages", totalPages}
// This closes the block opened above, such as a function, class, or control-flow branch.
        };
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This begins a reusable type or template: a blueprint for related data and behavior.
template <typename T>
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
PageResult<T> paginate(const std::vector<T>& allItems, int page = 1, int pageSize = 10) {
// This controls which part of the program runs next, based on the current condition or result.
    if (page < 1) page = 1;
// This controls which part of the program runs next, based on the current condition or result.
    if (pageSize < 1) pageSize = 10;

// This performs one step of the current operation, then continues to the next statement.
    PageResult<T> res;
// This creates or updates a value that the following logic uses.
    res.totalItems = static_cast<int>(allItems.size());
// This performs one step of the current operation, then continues to the next statement.
    res.pageSize = pageSize;
// This creates or updates a value that the following logic uses.
    res.totalPages = res.totalItems > 0 ? static_cast<int>(std::ceil(static_cast<double>(res.totalItems) / pageSize)) : 1;
// This performs one step of the current operation, then continues to the next statement.
    res.currentPage = (page > res.totalPages) ? res.totalPages : page;

// This creates or updates a value that the following logic uses.
    int startIndex = (res.currentPage - 1) * pageSize;
// This controls which part of the program runs next, based on the current condition or result.
    if (startIndex < res.totalItems) {
// This creates or updates a value that the following logic uses.
        int endIndex = std::min(startIndex + pageSize, res.totalItems);
// This controls which part of the program runs next, based on the current condition or result.
        for (int i = startIndex; i < endIndex; ++i) {
// This performs one step of the current operation, then continues to the next statement.
            res.items.push_back(allItems[i]);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This sends a result back to the code that called this function.
    return res;
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ──────────────────────────────────────────────
// This is a source comment that explains nearby code.
// DonorRepository: persists Donor profiles to CSV
// This is a source comment that explains nearby code.
// ──────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class DonorFileRepository final : public IRepository<Donor> {
// This creates or updates a value that the following logic uses.
    std::string filepath_;
// This creates or updates a value that the following logic uses.
    std::vector<Donor> donors_;
// This performs one step of the current operation, then continues to the next statement.
    mutable std::mutex mtx_;

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit DonorFileRepository(std::string filepath)
// This is part of the current expression or block; read it together with the nearby lines.
        : filepath_(std::move(filepath)) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void load() override {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        donors_.clear();
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::ifstream file(filepath_);
// This controls which part of the program runs next, based on the current condition or result.
        if (!file.is_open()) return;

// This creates or updates a value that the following logic uses.
        std::string line;
// This is part of the current expression or block; read it together with the nearby lines.
        std::getline(file, line); // header
// This controls which part of the program runs next, based on the current condition or result.
        while (std::getline(file, line)) {
// This controls which part of the program runs next, based on the current condition or result.
            if (line.empty()) continue;
// This creates or updates a value that the following logic uses.
            auto cols = splitCsv(line);
// This controls which part of the program runs next, based on the current condition or result.
            if (cols.size() < 8) continue;

// This performs one step of the current operation, then continues to the next statement.
            Donor donor(cols[0], cols[1], Credential{});
// This performs one step of the current operation, then continues to the next statement.
            donor.setAge(std::stoi(cols[2]));
// This performs one step of the current operation, then continues to the next statement.
            donor.setGender(cols[3]);
// This controls which part of the program runs next, based on the current condition or result.
            if (!cols[4].empty()) {
// This performs one step of the current operation, then continues to the next statement.
                donor.setBloodGroup(bloodGroupFromString(cols[4]));
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This performs one step of the current operation, then continues to the next statement.
            donor.setWeight(std::stod(cols[5]));
// This performs one step of the current operation, then continues to the next statement.
            donor.setPhone(cols[6]);
// This controls which part of the program runs next, based on the current condition or result.
            if (cols[7] == "true") donor.grantConsent();
// This controls which part of the program runs next, based on the current condition or result.
            else donor.revokeConsent();
// This performs one step of the current operation, then continues to the next statement.
            donor.markProfileComplete();

// This controls which part of the program runs next, based on the current condition or result.
            if (cols.size() >= 9) {
// This performs one step of the current operation, then continues to the next statement.
                donor.setLastDonationDate(cols[8]);
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This controls which part of the program runs next, based on the current condition or result.
            if (cols.size() >= 10 && !cols[9].empty()) {
// This performs one step of the current operation, then continues to the next statement.
                donor.setDonationCount(std::stoi(cols[9]));
// This closes the block opened above, such as a function, class, or control-flow branch.
            }

// This performs one step of the current operation, then continues to the next statement.
            donors_.push_back(std::move(donor));
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void save() override {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This is part of the current expression or block; read it together with the nearby lines.
        std::filesystem::create_directories(
// This performs one step of the current operation, then continues to the next statement.
            std::filesystem::path(filepath_).parent_path());
// This performs one step of the current operation, then continues to the next statement.
        std::ostringstream file;
// This performs one step of the current operation, then continues to the next statement.
        file << "username,fullName,age,gender,bloodGroup,weight,phone,consent,lastDonationDate,donationCount\n";
// This controls which part of the program runs next, based on the current condition or result.
        for (const auto& d : donors_) {
// This is part of the current expression or block; read it together with the nearby lines.
            file << csvField(d.username()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(d.fullName()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << d.age() << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(d.gender()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(d.bloodGroupString()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << d.weight() << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(d.phone()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << (d.hasConsent() ? "true" : "false") << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(d.lastDonationDate()) << ","
// This performs one step of the current operation, then continues to the next statement.
                 << d.donationCount() << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        atomicWrite(filepath_, file.str());
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This creates or updates a value that the following logic uses.
    std::vector<Donor>& all() override { return donors_; }
// This creates or updates a value that the following logic uses.
    const std::vector<Donor>& all() const override { return donors_; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::optional<std::reference_wrapper<Donor>> find(const std::string& username) override {
// This controls which part of the program runs next, based on the current condition or result.
        for (auto& d : donors_) {
// This controls which part of the program runs next, based on the current condition or result.
            if (d.username() == username) return d;
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return std::nullopt;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This creates or updates a value that the following logic uses.
    std::optional<std::reference_wrapper<const Donor>> find(
// This creates or updates a value that the following logic uses.
        const std::string& username) const override {
// This controls which part of the program runs next, based on the current condition or result.
        for (const auto& d : donors_) {
// This controls which part of the program runs next, based on the current condition or result.
            if (d.username() == username) return std::cref(d);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return std::nullopt;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void add(Donor donor) override {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        donors_.push_back(std::move(donor));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// ──────────────────────────────────────────────
// This is a source comment that explains nearby code.
// CampRepository: persists Camps to CSV
// This is a source comment that explains nearby code.
// ──────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class CampFileRepository final : public IRepository<Camp> {
// This creates or updates a value that the following logic uses.
    std::string filepath_;
// This creates or updates a value that the following logic uses.
    std::vector<Camp> camps_;
// This performs one step of the current operation, then continues to the next statement.
    mutable std::mutex mtx_;

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit CampFileRepository(std::string filepath)
// This is part of the current expression or block; read it together with the nearby lines.
        : filepath_(std::move(filepath)) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void load() override {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        camps_.clear();
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::ifstream file(filepath_);
// This controls which part of the program runs next, based on the current condition or result.
        if (!file.is_open()) return;

// This creates or updates a value that the following logic uses.
        std::string line;
// This is part of the current expression or block; read it together with the nearby lines.
        std::getline(file, line); // header
// This controls which part of the program runs next, based on the current condition or result.
        while (std::getline(file, line)) {
// This controls which part of the program runs next, based on the current condition or result.
            if (line.empty()) continue;
// This creates or updates a value that the following logic uses.
            auto cols = splitCsv(line);
// This controls which part of the program runs next, based on the current condition or result.
            if (cols.size() < 5) continue;
// This creates or updates a value that the following logic uses.
            int target = std::stoi(cols[3]);
// This creates or updates a value that the following logic uses.
            int collected = std::stoi(cols[4]);
// This creates or updates a value that the following logic uses.
            int slotCap = (cols.size() >= 6 && !cols[5].empty()) ? std::stoi(cols[5]) : target * 2;
// This performs one step of the current operation, then continues to the next statement.
            CampStatus status = (cols.size() >= 7) ? campStatusFromString(cols[6]) : CampStatus::Upcoming;
// This creates or updates a value that the following logic uses.
            int deferred = (cols.size() >= 8 && !cols[7].empty()) ? std::stoi(cols[7]) : 0;

// This performs one step of the current operation, then continues to the next statement.
            Camp camp(cols[0], cols[1], cols[2], target, slotCap, status, collected, deferred);

// This is a source comment that explains nearby code.
            // Parse registered donors list (semicolon separated in col 8)
// This controls which part of the program runs next, based on the current condition or result.
            if (cols.size() >= 9 && !cols[8].empty()) {
// This creates or updates a value that the following logic uses.
                auto reg = splitCsv(cols[8], ';');
// This controls which part of the program runs next, based on the current condition or result.
                for (const auto& r : reg) {
// This controls which part of the program runs next, based on the current condition or result.
                    if (!r.empty()) camp.addRegisteredDonorDirect(r);
// This closes the block opened above, such as a function, class, or control-flow branch.
                }
// This closes the block opened above, such as a function, class, or control-flow branch.
            }

// This performs one step of the current operation, then continues to the next statement.
            camps_.push_back(std::move(camp));
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void save() override {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This is part of the current expression or block; read it together with the nearby lines.
        std::filesystem::create_directories(
// This performs one step of the current operation, then continues to the next statement.
            std::filesystem::path(filepath_).parent_path());
// This performs one step of the current operation, then continues to the next statement.
        std::ostringstream file;
// This performs one step of the current operation, then continues to the next statement.
        file << "id,name,date,targetUnits,unitsCollected,slotCapacity,status,deferredCount,registeredDonors\n";
// This controls which part of the program runs next, based on the current condition or result.
        for (const auto& c : camps_) {
// This creates or updates a value that the following logic uses.
            std::string regStr;
// This controls which part of the program runs next, based on the current condition or result.
            for (size_t i = 0; i < c.registeredDonors().size(); ++i) {
// This controls which part of the program runs next, based on the current condition or result.
                if (i > 0) regStr += ";";
// This performs one step of the current operation, then continues to the next statement.
                regStr += c.registeredDonors()[i];
// This closes the block opened above, such as a function, class, or control-flow branch.
            }

// This is part of the current expression or block; read it together with the nearby lines.
            file << csvField(c.id()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(c.name()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(c.date()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << c.targetUnits() << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << c.unitsCollected() << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << c.slotCapacity() << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(c.statusString()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << c.deferredCount() << ","
// This performs one step of the current operation, then continues to the next statement.
                 << csvField(regStr) << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        atomicWrite(filepath_, file.str());
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This creates or updates a value that the following logic uses.
    std::vector<Camp>& all() override { return camps_; }
// This creates or updates a value that the following logic uses.
    const std::vector<Camp>& all() const override { return camps_; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::optional<std::reference_wrapper<Camp>> find(const std::string& id) override {
// This controls which part of the program runs next, based on the current condition or result.
        for (auto& c : camps_) {
// This controls which part of the program runs next, based on the current condition or result.
            if (c.id() == id) return c;
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return std::nullopt;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This creates or updates a value that the following logic uses.
    std::optional<std::reference_wrapper<const Camp>> find(
// This creates or updates a value that the following logic uses.
        const std::string& id) const override {
// This controls which part of the program runs next, based on the current condition or result.
        for (const auto& c : camps_) {
// This controls which part of the program runs next, based on the current condition or result.
            if (c.id() == id) return std::cref(c);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return std::nullopt;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void add(Camp camp) override {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        camps_.push_back(std::move(camp));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// ──────────────────────────────────────────────
// This is a source comment that explains nearby code.
// DonationRepository: persists Donations to CSV
// This is a source comment that explains nearby code.
// ──────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class DonationFileRepository final : public IRepository<Donation> {
// This creates or updates a value that the following logic uses.
    std::string filepath_;
// This creates or updates a value that the following logic uses.
    std::vector<Donation> donations_;
// This performs one step of the current operation, then continues to the next statement.
    mutable std::mutex mtx_;

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit DonationFileRepository(std::string filepath)
// This is part of the current expression or block; read it together with the nearby lines.
        : filepath_(std::move(filepath)) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void load() override {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        donations_.clear();
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::ifstream file(filepath_);
// This controls which part of the program runs next, based on the current condition or result.
        if (!file.is_open()) return;

// This creates or updates a value that the following logic uses.
        std::string line;
// This is part of the current expression or block; read it together with the nearby lines.
        std::getline(file, line); // header
// This controls which part of the program runs next, based on the current condition or result.
        while (std::getline(file, line)) {
// This controls which part of the program runs next, based on the current condition or result.
            if (line.empty()) continue;
// This creates or updates a value that the following logic uses.
            auto cols = splitCsv(line);
// This controls which part of the program runs next, based on the current condition or result.
            if (cols.size() < 8) continue;
// This is part of the current expression or block; read it together with the nearby lines.
            donations_.emplace_back(
// This is part of the current expression or block; read it together with the nearby lines.
                cols[0], cols[1], cols[2], cols[3],
// This performs one step of the current operation, then continues to the next statement.
                cols[4], cols[5], cols[6], cols[7]);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void save() override {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This is part of the current expression or block; read it together with the nearby lines.
        std::filesystem::create_directories(
// This performs one step of the current operation, then continues to the next statement.
            std::filesystem::path(filepath_).parent_path());
// This performs one step of the current operation, then continues to the next statement.
        std::ostringstream file;
// This performs one step of the current operation, then continues to the next statement.
        file << "id,donorUsername,donorName,bloodGroup,campId,campName,date,status\n";
// This controls which part of the program runs next, based on the current condition or result.
        for (const auto& d : donations_) {
// This is part of the current expression or block; read it together with the nearby lines.
            file << csvField(d.id()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(d.donorUsername()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(d.donorName()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(d.bloodGroup()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(d.campId()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(d.campName()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(d.date()) << ","
// This performs one step of the current operation, then continues to the next statement.
                 << csvField(d.status()) << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        atomicWrite(filepath_, file.str());
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This creates or updates a value that the following logic uses.
    std::vector<Donation>& all() override { return donations_; }
// This creates or updates a value that the following logic uses.
    const std::vector<Donation>& all() const override { return donations_; }

// This is part of the current expression or block; read it together with the nearby lines.
    std::optional<std::reference_wrapper<Donation>> find(
// This creates or updates a value that the following logic uses.
        const std::string& id) override {
// This controls which part of the program runs next, based on the current condition or result.
        for (auto& d : donations_) {
// This controls which part of the program runs next, based on the current condition or result.
            if (d.id() == id) return d;
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return std::nullopt;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This creates or updates a value that the following logic uses.
    std::optional<std::reference_wrapper<const Donation>> find(
// This creates or updates a value that the following logic uses.
        const std::string& id) const override {
// This controls which part of the program runs next, based on the current condition or result.
        for (const auto& d : donations_) {
// This controls which part of the program runs next, based on the current condition or result.
            if (d.id() == id) return std::cref(d);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return std::nullopt;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void add(Donation donation) override {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        donations_.push_back(std::move(donation));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// ──────────────────────────────────────────────
// This is a source comment that explains nearby code.
// AccountRepository: persists Account credentials
// This is a source comment that explains nearby code.
// ──────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class AccountFileRepository {
// This creates or updates a value that the following logic uses.
    std::string filepath_;
// This creates or updates a value that the following logic uses.
    std::vector<std::unique_ptr<Account>> accounts_;
// This performs one step of the current operation, then continues to the next statement.
    mutable std::mutex mtx_;

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit AccountFileRepository(std::string filepath)
// This is part of the current expression or block; read it together with the nearby lines.
        : filepath_(std::move(filepath)) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void load() {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        accounts_.clear();
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::ifstream file(filepath_);
// This controls which part of the program runs next, based on the current condition or result.
        if (!file.is_open()) return;

// This creates or updates a value that the following logic uses.
        std::string line;
// This is part of the current expression or block; read it together with the nearby lines.
        std::getline(file, line); // header
// This controls which part of the program runs next, based on the current condition or result.
        while (std::getline(file, line)) {
// This controls which part of the program runs next, based on the current condition or result.
            if (line.empty()) continue;
// This creates or updates a value that the following logic uses.
            auto cols = splitCsv(line);
// This controls which part of the program runs next, based on the current condition or result.
            if (cols.size() < 5) continue;
// This is a source comment that explains nearby code.
            // username,fullName,role,salt,hash,orgType,approvalStatus,city
// This creates or updates a value that the following logic uses.
            auto cred = Credential::fromStored(cols[3], cols[4]);
// This creates or updates a value that the following logic uses.
            std::string role = cols[2];

// This controls which part of the program runs next, based on the current condition or result.
            if (role == "donor") {
// This is part of the current expression or block; read it together with the nearby lines.
                accounts_.push_back(
// This performs one step of the current operation, then continues to the next statement.
                    std::make_unique<Donor>(cols[0], cols[1], std::move(cred)));
// This closes the block opened above, such as a function, class, or control-flow branch.
            } else if (role == "admin") {
// This is part of the current expression or block; read it together with the nearby lines.
                accounts_.push_back(
// This performs one step of the current operation, then continues to the next statement.
                    std::make_unique<Admin>(cols[0], cols[1], std::move(cred)));
// This closes the block opened above, such as a function, class, or control-flow branch.
            } else {
// This creates or updates a value that the following logic uses.
                std::string orgType = (cols.size() >= 6 && !cols[5].empty()) ? cols[5] : "ngo";
// This creates or updates a value that the following logic uses.
                std::string approval = (cols.size() >= 7 && !cols[6].empty()) ? cols[6] : "Approved";
// This creates or updates a value that the following logic uses.
                std::string city = (cols.size() >= 8 && !cols[7].empty()) ? cols[7] : "Nainital";
// This creates or updates a value that the following logic uses.
                std::string phone = (cols.size() >= 9 && !cols[8].empty()) ? cols[8] : "05942-235022";
// This creates or updates a value that the following logic uses.
                std::string email = (cols.size() >= 10 && !cols[9].empty()) ? cols[9] : "bloodbank@ramsayhospital.org";

// This controls which part of the program runs next, based on the current condition or result.
                if (orgType == "hospital") {
// This is part of the current expression or block; read it together with the nearby lines.
                    accounts_.push_back(
// This performs one step of the current operation, then continues to the next statement.
                        std::make_unique<Hospital>(cols[0], cols[1], std::move(cred), approval, city, phone, email));
// This closes the block opened above, such as a function, class, or control-flow branch.
                } else {
// This is part of the current expression or block; read it together with the nearby lines.
                    accounts_.push_back(
// This performs one step of the current operation, then continues to the next statement.
                        std::make_unique<NGO>(cols[0], cols[1], std::move(cred), approval, city, phone, email));
// This closes the block opened above, such as a function, class, or control-flow branch.
                }
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void save() {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This is part of the current expression or block; read it together with the nearby lines.
        std::filesystem::create_directories(
// This performs one step of the current operation, then continues to the next statement.
            std::filesystem::path(filepath_).parent_path());
// This performs one step of the current operation, then continues to the next statement.
        std::ostringstream file;
// This performs one step of the current operation, then continues to the next statement.
        file << "username,fullName,role,salt,hash,orgType,approvalStatus,city,phone,email\n";
// This controls which part of the program runs next, based on the current condition or result.
        for (const auto& acc : accounts_) {
// This creates or updates a value that the following logic uses.
            std::string orgType = "";
// This creates or updates a value that the following logic uses.
            std::string approval = "Approved";
// This creates or updates a value that the following logic uses.
            std::string city = "Nainital";
// This creates or updates a value that the following logic uses.
            std::string phone = "";
// This creates or updates a value that the following logic uses.
            std::string email = "";

// This creates or updates a value that the following logic uses.
            auto* org = dynamic_cast<Organization*>(acc.get());
// This controls which part of the program runs next, based on the current condition or result.
            if (org) {
// This performs one step of the current operation, then continues to the next statement.
                approval = org->approvalStatus();
// This performs one step of the current operation, then continues to the next statement.
                city = org->city();
// This performs one step of the current operation, then continues to the next statement.
                phone = org->contactPhone();
// This performs one step of the current operation, then continues to the next statement.
                email = org->contactEmail();
// This performs one step of the current operation, then continues to the next statement.
                orgType = (dynamic_cast<Hospital*>(org) != nullptr) ? "hospital" : "ngo";
// This closes the block opened above, such as a function, class, or control-flow branch.
            }

// This is part of the current expression or block; read it together with the nearby lines.
            file << csvField(acc->username()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(acc->fullName()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(acc->role()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << acc->credential().salt() << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << acc->credential().hash() << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(orgType) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(approval) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(city) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(phone) << ","
// This performs one step of the current operation, then continues to the next statement.
                 << csvField(email) << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        atomicWrite(filepath_, file.str());
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This creates or updates a value that the following logic uses.
    std::vector<std::unique_ptr<Account>>& all() { return accounts_; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    Account* findByUsername(const std::string& username) {
// This controls which part of the program runs next, based on the current condition or result.
        for (auto& acc : accounts_) {
// This controls which part of the program runs next, based on the current condition or result.
            if (acc->username() == username) return acc.get();
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return nullptr;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    const Account* findByUsername(const std::string& username) const {
// This controls which part of the program runs next, based on the current condition or result.
        for (const auto& acc : accounts_) {
// This controls which part of the program runs next, based on the current condition or result.
            if (acc->username() == username) return acc.get();
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return nullptr;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void add(std::unique_ptr<Account> account) {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        accounts_.push_back(std::move(account));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// ──────────────────────────────────────────────
// This is a source comment that explains nearby code.
// RequestFileRepository: persists Blood Requests
// This is a source comment that explains nearby code.
// ──────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class RequestFileRepository {
// This creates or updates a value that the following logic uses.
    std::string filepath_;
// This creates or updates a value that the following logic uses.
    std::vector<std::unique_ptr<Request>> requests_;
// This performs one step of the current operation, then continues to the next statement.
    mutable std::mutex mtx_;

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit RequestFileRepository(std::string filepath)
// This is part of the current expression or block; read it together with the nearby lines.
        : filepath_(std::move(filepath)) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void load() {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        requests_.clear();
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::ifstream file(filepath_);
// This controls which part of the program runs next, based on the current condition or result.
        if (!file.is_open()) return;

// This creates or updates a value that the following logic uses.
        std::string line;
// This is part of the current expression or block; read it together with the nearby lines.
        std::getline(file, line); // header
// This controls which part of the program runs next, based on the current condition or result.
        while (std::getline(file, line)) {
// This controls which part of the program runs next, based on the current condition or result.
            if (line.empty()) continue;
// This creates or updates a value that the following logic uses.
            auto cols = splitCsv(line);
// This controls which part of the program runs next, based on the current condition or result.
            if (cols.size() < 8) continue;
// This is a source comment that explains nearby code.
            // id,orgUsername,hospitalName,bloodGroup,unitsNeeded,status,urgency,createdAt,responders
// This performs one step of the current operation, then continues to the next statement.
            BloodGroup bg = bloodGroupFromString(cols[3]);
// This creates or updates a value that the following logic uses.
            int needed = std::stoi(cols[4]);
// This performs one step of the current operation, then continues to the next statement.
            RequestStatus st = requestStatusFromString(cols[5]);
// This creates or updates a value that the following logic uses.
            std::string urgency = cols[6];
// This creates or updates a value that the following logic uses.
            std::string createdAt = cols[7];

// This creates or updates a value that the following logic uses.
            std::unique_ptr<Request> req;
// This controls which part of the program runs next, based on the current condition or result.
            if (urgency == "Critical") {
// This performs one step of the current operation, then continues to the next statement.
                req = std::make_unique<UrgentRequest>(cols[0], cols[1], cols[2], bg, needed, true, createdAt, st);
// This closes the block opened above, such as a function, class, or control-flow branch.
            } else if (urgency == "Urgent") {
// This performs one step of the current operation, then continues to the next statement.
                req = std::make_unique<UrgentRequest>(cols[0], cols[1], cols[2], bg, needed, false, createdAt, st);
// This closes the block opened above, such as a function, class, or control-flow branch.
            } else {
// This performs one step of the current operation, then continues to the next statement.
                req = std::make_unique<RoutineRequest>(cols[0], cols[1], cols[2], bg, needed, createdAt, st);
// This closes the block opened above, such as a function, class, or control-flow branch.
            }

// This controls which part of the program runs next, based on the current condition or result.
            if (cols.size() >= 9 && !cols[8].empty()) {
// This creates or updates a value that the following logic uses.
                auto resps = splitCsv(cols[8], ';');
// This controls which part of the program runs next, based on the current condition or result.
                for (const auto& r : resps) {
// This controls which part of the program runs next, based on the current condition or result.
                    if (!r.empty()) req->restoreResponder(r);
// This closes the block opened above, such as a function, class, or control-flow branch.
                }
// This closes the block opened above, such as a function, class, or control-flow branch.
            }

// This performs one step of the current operation, then continues to the next statement.
            requests_.push_back(std::move(req));
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void save() {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This is part of the current expression or block; read it together with the nearby lines.
        std::filesystem::create_directories(
// This performs one step of the current operation, then continues to the next statement.
            std::filesystem::path(filepath_).parent_path());
// This performs one step of the current operation, then continues to the next statement.
        std::ostringstream file;
// This performs one step of the current operation, then continues to the next statement.
        file << "id,orgUsername,hospitalName,bloodGroup,unitsNeeded,status,urgency,createdAt,responders\n";
// This controls which part of the program runs next, based on the current condition or result.
        for (const auto& r : requests_) {
// This creates or updates a value that the following logic uses.
            std::string respStr;
// This controls which part of the program runs next, based on the current condition or result.
            for (size_t i = 0; i < r->responders().size(); ++i) {
// This controls which part of the program runs next, based on the current condition or result.
                if (i > 0) respStr += ";";
// This performs one step of the current operation, then continues to the next statement.
                respStr += r->responders()[i];
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This is part of the current expression or block; read it together with the nearby lines.
            file << csvField(r->id()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(r->orgUsername()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(r->hospitalName()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(r->bloodGroupString()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << r->unitsNeeded() << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(r->statusString()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(r->urgency()) << ","
// This is part of the current expression or block; read it together with the nearby lines.
                 << csvField(r->createdAt()) << ","
// This performs one step of the current operation, then continues to the next statement.
                 << csvField(respStr) << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        atomicWrite(filepath_, file.str());
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This creates or updates a value that the following logic uses.
    std::vector<std::unique_ptr<Request>>& all() { return requests_; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    Request* find(const std::string& id) {
// This controls which part of the program runs next, based on the current condition or result.
        for (auto& r : requests_) {
// This controls which part of the program runs next, based on the current condition or result.
            if (r->id() == id) return r.get();
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return nullptr;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void add(std::unique_ptr<Request> req) {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        requests_.push_back(std::move(req));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
