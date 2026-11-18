// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/Donor.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in "bloodline/Account.hpp", so this file can use its declarations.
#include "bloodline/Account.hpp"
// This brings in "bloodline/BloodGroup.hpp", so this file can use its declarations.
#include "bloodline/BloodGroup.hpp"
// This brings in "bloodline/Errors.hpp", so this file can use its declarations.
#include "bloodline/Errors.hpp"
// This brings in "bloodline/DateUtil.hpp", so this file can use its declarations.
#include "bloodline/DateUtil.hpp"
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <vector>, so this file can use its declarations.
#include <vector>
// This brings in <optional>, so this file can use its declarations.
#include <optional>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// OOP: INHERITANCE — Donor derives from Account.
// This is a source comment that explains nearby code.
// OOP: ENCAPSULATION — All data is private. Setters validate invariants.
// This is a source comment that explains nearby code.
// Waiting period and donor safety calculations live encapsulated within Donor.

// This begins a reusable type or template: a blueprint for related data and behavior.
class Donor final : public Account {
// This creates or updates a value that the following logic uses.
    int age_ = 0;
// This creates or updates a value that the following logic uses.
    std::string gender_;
// This performs one step of the current operation, then continues to the next statement.
    std::optional<BloodGroup> blood_group_;
// This creates or updates a value that the following logic uses.
    double weight_ = 0.0;
// This creates or updates a value that the following logic uses.
    std::string phone_;
// This creates or updates a value that the following logic uses.
    bool consent_ = false;
// This creates or updates a value that the following logic uses.
    bool profile_complete_ = false;
// This creates or updates a value that the following logic uses.
    std::string last_donation_date_;
// This creates or updates a value that the following logic uses.
    int donation_count_ = 0;

// The following members are available to other parts of the program.
public:
// This is a source comment that explains nearby code.
    // Configurable standard waiting periods
// This creates or updates a value that the following logic uses.
    static constexpr int DEFAULT_WAITING_PERIOD_MALE = 90;
// This creates or updates a value that the following logic uses.
    static constexpr int DEFAULT_WAITING_PERIOD_FEMALE = 120;

// This creates or updates a value that the following logic uses.
    Donor(std::string username, std::string fullName, Credential credential)
// This is part of the current expression or block; read it together with the nearby lines.
        : Account(std::move(username), std::move(fullName), std::move(credential)) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string dashboardKind() const override { return "donor"; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string displayName() const override { return fullName(); }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string role() const override { return "donor"; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    nlohmann::json toJson() const override {
// This sends a result back to the code that called this function.
        return {
// This is part of the current expression or block; read it together with the nearby lines.
            {"username", username()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"fullName", fullName()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"role", role()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"dashboardKind", dashboardKind()}
// This closes the block opened above, such as a function, class, or control-flow branch.
        };
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Profile data accessors
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int age() const { return age_; }
// This creates or updates a value that the following logic uses.
    const std::string& gender() const { return gender_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    double weight() const { return weight_; }
// This creates or updates a value that the following logic uses.
    const std::string& phone() const { return phone_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool hasConsent() const { return consent_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool isProfileComplete() const { return profile_complete_; }
// This creates or updates a value that the following logic uses.
    const std::string& lastDonationDate() const { return last_donation_date_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int donationCount() const { return donation_count_; }

// This creates or updates a value that the following logic uses.
    std::optional<BloodGroup> bloodGroup() const { return blood_group_; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string bloodGroupString() const {
// This sends a result back to the code that called this function.
        return blood_group_ ? bloodGroupToString(*blood_group_) : "";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Encapsulation: Waiting period based on biological guidelines
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int waitingPeriodDays() const {
// This controls which part of the program runs next, based on the current condition or result.
        if (gender_ == "Female") return DEFAULT_WAITING_PERIOD_FEMALE;
// This sends a result back to the code that called this function.
        return DEFAULT_WAITING_PERIOD_MALE;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string nextEligibleDate() const {
// This controls which part of the program runs next, based on the current condition or result.
        if (last_donation_date_.empty()) return "";
// This sends a result back to the code that called this function.
        return DateUtil::addDays(last_donation_date_, waitingPeriodDays());
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool isEligibleOnDate(const std::string& targetDate = "") const {
// This controls which part of the program runs next, based on the current condition or result.
        if (last_donation_date_.empty()) return true;
// This creates or updates a value that the following logic uses.
        std::string refDate = targetDate.empty() ? DateUtil::today() : targetDate;
// This sends a result back to the code that called this function.
        return refDate >= nextEligibleDate();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int daysUntilEligible(const std::string& targetDate = "") const {
// This controls which part of the program runs next, based on the current condition or result.
        if (last_donation_date_.empty()) return 0;
// This creates or updates a value that the following logic uses.
        std::string refDate = targetDate.empty() ? DateUtil::today() : targetDate;
// This creates or updates a value that the following logic uses.
        int diff = DateUtil::daysBetween(refDate, nextEligibleDate());
// This sends a result back to the code that called this function.
        return diff > 0 ? diff : 0;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Privacy security: phone masking for public and organization views
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string maskedPhone() const {
// This controls which part of the program runs next, based on the current condition or result.
        if (phone_.length() <= 4) return "****";
// This creates or updates a value that the following logic uses.
        std::string masked = phone_;
// This performs one step of the current operation, then continues to the next statement.
        size_t start = (phone_.length() > 6) ? 2 : 1;
// This performs one step of the current operation, then continues to the next statement.
        size_t end = (phone_.length() > 6) ? phone_.length() - 2 : phone_.length() - 1;
// This controls which part of the program runs next, based on the current condition or result.
        for (size_t i = start; i < end; ++i) {
// This performs one step of the current operation, then continues to the next statement.
            masked[i] = '*';
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return masked;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::vector<std::string> milestoneBadges() const {
// This creates or updates a value that the following logic uses.
        std::vector<std::string> badges;
// This controls which part of the program runs next, based on the current condition or result.
        if (donation_count_ >= 10) badges.push_back("Gold Donor (10+ Donations)");
// This controls which part of the program runs next, based on the current condition or result.
        if (donation_count_ >= 5) badges.push_back("Silver Donor (5+ Donations)");
// This controls which part of the program runs next, based on the current condition or result.
        if (donation_count_ >= 1) badges.push_back("Bronze Donor (Milestone 1)");
// This sends a result back to the code that called this function.
        return badges;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Validated setters enforce domain invariants
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setAge(int age) {
// This controls which part of the program runs next, based on the current condition or result.
        if (age < 0 || age > 150) {
// This reports an error so the caller can handle the failed operation.
            throw BadRequestError("Age must be between 0 and 150");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        age_ = age;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setGender(const std::string& gender) {
// This controls which part of the program runs next, based on the current condition or result.
        if (gender != "Male" && gender != "Female" && gender != "Other") {
// This reports an error so the caller can handle the failed operation.
            throw BadRequestError("Gender must be Male, Female, or Other");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        gender_ = gender;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setBloodGroup(BloodGroup bg) {
// This performs one step of the current operation, then continues to the next statement.
        blood_group_ = bg;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setWeight(double weight) {
// This controls which part of the program runs next, based on the current condition or result.
        if (weight < 20.0 || weight > 300.0) {
// This reports an error so the caller can handle the failed operation.
            throw BadRequestError("Weight must be between 20 and 300 kg");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        weight_ = weight;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setPhone(const std::string& phone) {
// This controls which part of the program runs next, based on the current condition or result.
        if (phone.empty()) {
// This reports an error so the caller can handle the failed operation.
            throw BadRequestError("Phone number cannot be empty");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        phone_ = phone;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setLastDonationDate(const std::string& date) {
// This performs one step of the current operation, then continues to the next statement.
        last_donation_date_ = date;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setDonationCount(int count) {
// This performs one step of the current operation, then continues to the next statement.
        donation_count_ = count;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void incrementDonationCount(const std::string& donationDate) {
// This performs one step of the current operation, then continues to the next statement.
        donation_count_++;
// This controls which part of the program runs next, based on the current condition or result.
        if (last_donation_date_.empty() || donationDate > last_donation_date_) {
// This performs one step of the current operation, then continues to the next statement.
            last_donation_date_ = donationDate;
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void grantConsent() { consent_ = true; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void revokeConsent() { consent_ = false; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void markProfileComplete() { profile_complete_ = true; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    nlohmann::json profileToJson(bool maskPhone = false) const {
// This sends a result back to the code that called this function.
        return {
// This is part of the current expression or block; read it together with the nearby lines.
            {"username", username()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"fullName", fullName()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"age", age_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"gender", gender_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"bloodGroup", bloodGroupString()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"weight", weight_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"phone", maskPhone ? maskedPhone() : phone_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"consent", consent_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"profileComplete", profile_complete_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"lastDonationDate", last_donation_date_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"nextEligibleDate", nextEligibleDate()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"isEligibleToday", isEligibleOnDate()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"daysUntilEligible", daysUntilEligible()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"waitingPeriodDays", waitingPeriodDays()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"donationCount", donation_count_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"milestoneBadges", milestoneBadges()}
// This closes the block opened above, such as a function, class, or control-flow branch.
        };
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool operator==(const Donor& other) const {
// This sends a result back to the code that called this function.
        return username() == other.username();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool operator<(const Donor& other) const {
// This sends a result back to the code that called this function.
        return username() < other.username();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
