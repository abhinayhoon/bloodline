#pragma once
#include "bloodline/Account.hpp"
#include <string>

namespace bloodline {

// OOP: INHERITANCE (multi-level) — Organization derives from Account.
// Hospital and NGO specialize Organization with different display behavior.
// Added approval status ("Pending", "Approved", "Rejected"), city, and contact details.

class Organization : public Account {
    std::string org_type_;
    std::string approval_status_ = "Pending";
    std::string city_ = "Nainital";
    std::string contactPhone_ = "";
    std::string contactEmail_ = "";

protected:
    const std::string& orgType() const { return org_type_; }

public:
    Organization(std::string username, std::string fullName,
                 Credential credential, std::string orgType,
                 std::string approvalStatus = "Pending",
                 std::string city = "Nainital",
                 std::string phone = "",
                 std::string email = "")
        : Account(std::move(username), std::move(fullName), std::move(credential)),
          org_type_(std::move(orgType)),
          approval_status_(std::move(approvalStatus)),
          city_(std::move(city)),
          contactPhone_(std::move(phone)),
          contactEmail_(std::move(email)) {}

    std::string role() const override { return "ngo"; }

    const std::string& approvalStatus() const { return approval_status_; }
    bool isApproved() const { return approval_status_ == "Approved"; }
    void setApprovalStatus(const std::string& status) { approval_status_ = status; }

    const std::string& city() const { return city_; }
    void setCity(const std::string& city) { city_ = city; }

    const std::string& contactPhone() const { return contactPhone_; }
    void setContactPhone(const std::string& phone) { contactPhone_ = phone; }

    const std::string& contactEmail() const { return contactEmail_; }
    void setContactEmail(const std::string& email) { contactEmail_ = email; }

    nlohmann::json toJson() const override {
        return {
            {"username", username()},
            {"fullName", fullName()},
            {"role", role()},
            {"dashboardKind", dashboardKind()},
            {"orgType", org_type_},
            {"approvalStatus", approval_status_},
            {"city", city_},
            {"contactPhone", contactPhone_},
            {"contactEmail", contactEmail_}
        };
    }
};

// OOP: INHERITANCE — Hospital specializes Organization
class Hospital final : public Organization {
public:
    Hospital(std::string username, std::string fullName, Credential credential,
             std::string approvalStatus = "Approved", std::string city = "Nainital",
             std::string phone = "", std::string email = "")
        : Organization(std::move(username), std::move(fullName),
                       std::move(credential), "hospital",
                       std::move(approvalStatus), std::move(city),
                       std::move(phone), std::move(email)) {}

    std::string dashboardKind() const override { return "ngo"; }

    std::string displayName() const override {
        return fullName() + " (Hospital)";
    }
};

// OOP: INHERITANCE — NGO specializes Organization
class NGO final : public Organization {
public:
    NGO(std::string username, std::string fullName, Credential credential,
        std::string approvalStatus = "Approved", std::string city = "Nainital",
        std::string phone = "", std::string email = "")
        : Organization(std::move(username), std::move(fullName),
                       std::move(credential), "ngo",
                       std::move(approvalStatus), std::move(city),
                       std::move(phone), std::move(email)) {}

    std::string dashboardKind() const override { return "ngo"; }

    std::string displayName() const override {
        return fullName() + " (NGO)";
    }
};

} // namespace bloodline
