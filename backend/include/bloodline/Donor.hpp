#pragma once
#include "bloodline/Account.hpp"
#include "bloodline/BloodGroup.hpp"
#include "bloodline/Errors.hpp"
#include "bloodline/DateUtil.hpp"
#include <string>
#include <vector>
#include <optional>

namespace bloodline {

// OOP: INHERITANCE — Donor derives from Account.
// OOP: ENCAPSULATION — All data is private. Setters validate invariants.
// Waiting period and donor safety calculations live encapsulated within Donor.

class Donor final : public Account {
    int age_ = 0;
    std::string gender_;
    std::optional<BloodGroup> blood_group_;
    double weight_ = 0.0;
    std::string phone_;
    bool consent_ = false;
    bool profile_complete_ = false;
    std::string last_donation_date_;
    int donation_count_ = 0;

public:
    // Configurable standard waiting periods
    static constexpr int DEFAULT_WAITING_PERIOD_MALE = 90;
    static constexpr int DEFAULT_WAITING_PERIOD_FEMALE = 120;

    Donor(std::string username, std::string fullName, Credential credential)
        : Account(std::move(username), std::move(fullName), std::move(credential)) {}

    std::string dashboardKind() const override { return "donor"; }
    std::string displayName() const override { return fullName(); }
    std::string role() const override { return "donor"; }

    nlohmann::json toJson() const override {
        return {
            {"username", username()},
            {"fullName", fullName()},
            {"role", role()},
            {"dashboardKind", dashboardKind()}
        };
    }

    // Profile data accessors
    int age() const { return age_; }
    const std::string& gender() const { return gender_; }
    double weight() const { return weight_; }
    const std::string& phone() const { return phone_; }
    bool hasConsent() const { return consent_; }
    bool isProfileComplete() const { return profile_complete_; }
    const std::string& lastDonationDate() const { return last_donation_date_; }
    int donationCount() const { return donation_count_; }

    std::optional<BloodGroup> bloodGroup() const { return blood_group_; }

    std::string bloodGroupString() const {
        return blood_group_ ? bloodGroupToString(*blood_group_) : "";
    }

    // Encapsulation: Waiting period based on biological guidelines
    int waitingPeriodDays() const {
        if (gender_ == "Female") return DEFAULT_WAITING_PERIOD_FEMALE;
        return DEFAULT_WAITING_PERIOD_MALE;
    }

    std::string nextEligibleDate() const {
        if (last_donation_date_.empty()) return "";
        return DateUtil::addDays(last_donation_date_, waitingPeriodDays());
    }

    bool isEligibleOnDate(const std::string& targetDate = "") const {
        if (last_donation_date_.empty()) return true;
        std::string refDate = targetDate.empty() ? DateUtil::today() : targetDate;
        return refDate >= nextEligibleDate();
    }

    int daysUntilEligible(const std::string& targetDate = "") const {
        if (last_donation_date_.empty()) return 0;
        std::string refDate = targetDate.empty() ? DateUtil::today() : targetDate;
        int diff = DateUtil::daysBetween(refDate, nextEligibleDate());
        return diff > 0 ? diff : 0;
    }

    // Privacy security: phone masking for public and organization views
    std::string maskedPhone() const {
        if (phone_.length() <= 4) return "****";
        std::string masked = phone_;
        size_t start = (phone_.length() > 6) ? 2 : 1;
        size_t end = (phone_.length() > 6) ? phone_.length() - 2 : phone_.length() - 1;
        for (size_t i = start; i < end; ++i) {
            masked[i] = '*';
        }
        return masked;
    }

    std::vector<std::string> milestoneBadges() const {
        std::vector<std::string> badges;
        if (donation_count_ >= 10) badges.push_back("Gold Donor (10+ Donations)");
        if (donation_count_ >= 5) badges.push_back("Silver Donor (5+ Donations)");
        if (donation_count_ >= 1) badges.push_back("Bronze Donor (Milestone 1)");
        return badges;
    }

    // Validated setters enforce domain invariants
    void setAge(int age) {
        if (age < 0 || age > 150) {
            throw BadRequestError("Age must be between 0 and 150");
        }
        age_ = age;
    }

    void setGender(const std::string& gender) {
        if (gender != "Male" && gender != "Female" && gender != "Other") {
            throw BadRequestError("Gender must be Male, Female, or Other");
        }
        gender_ = gender;
    }

    void setBloodGroup(BloodGroup bg) {
        blood_group_ = bg;
    }

    void setWeight(double weight) {
        if (weight < 20.0 || weight > 300.0) {
            throw BadRequestError("Weight must be between 20 and 300 kg");
        }
        weight_ = weight;
    }

    void setPhone(const std::string& phone) {
        if (phone.empty()) {
            throw BadRequestError("Phone number cannot be empty");
        }
        phone_ = phone;
    }

    void setLastDonationDate(const std::string& date) {
        last_donation_date_ = date;
    }

    void setDonationCount(int count) {
        donation_count_ = count;
    }

    void incrementDonationCount(const std::string& donationDate) {
        donation_count_++;
        if (last_donation_date_.empty() || donationDate > last_donation_date_) {
            last_donation_date_ = donationDate;
        }
    }

    void grantConsent() { consent_ = true; }
    void revokeConsent() { consent_ = false; }
    void markProfileComplete() { profile_complete_ = true; }

    nlohmann::json profileToJson(bool maskPhone = false) const {
        return {
            {"username", username()},
            {"fullName", fullName()},
            {"age", age_},
            {"gender", gender_},
            {"bloodGroup", bloodGroupString()},
            {"weight", weight_},
            {"phone", maskPhone ? maskedPhone() : phone_},
            {"consent", consent_},
            {"profileComplete", profile_complete_},
            {"lastDonationDate", last_donation_date_},
            {"nextEligibleDate", nextEligibleDate()},
            {"isEligibleToday", isEligibleOnDate()},
            {"daysUntilEligible", daysUntilEligible()},
            {"waitingPeriodDays", waitingPeriodDays()},
            {"donationCount", donation_count_},
            {"milestoneBadges", milestoneBadges()}
        };
    }

    bool operator==(const Donor& other) const {
        return username() == other.username();
    }

    bool operator<(const Donor& other) const {
        return username() < other.username();
    }
};

} // namespace bloodline
