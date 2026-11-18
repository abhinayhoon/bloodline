// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/Organization.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in "bloodline/Account.hpp", so this file can use its declarations.
#include "bloodline/Account.hpp"
// This brings in <string>, so this file can use its declarations.
#include <string>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// OOP: INHERITANCE (multi-level) — Organization derives from Account.
// This is a source comment that explains nearby code.
// Hospital and NGO specialize Organization with different display behavior.
// This is a source comment that explains nearby code.
// Added approval status ("Pending", "Approved", "Rejected"), city, and contact details.

// This begins a reusable type or template: a blueprint for related data and behavior.
class Organization : public Account {
// This creates or updates a value that the following logic uses.
    std::string org_type_;
// This creates or updates a value that the following logic uses.
    std::string approval_status_ = "Pending";
// This creates or updates a value that the following logic uses.
    std::string city_ = "Nainital";
// This creates or updates a value that the following logic uses.
    std::string contactPhone_ = "";
// This creates or updates a value that the following logic uses.
    std::string contactEmail_ = "";

// The following members are shared with this class and its child classes.
protected:
// This creates or updates a value that the following logic uses.
    const std::string& orgType() const { return org_type_; }

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    Organization(std::string username, std::string fullName,
// This creates or updates a value that the following logic uses.
                 Credential credential, std::string orgType,
// This creates or updates a value that the following logic uses.
                 std::string approvalStatus = "Pending",
// This creates or updates a value that the following logic uses.
                 std::string city = "Nainital",
// This creates or updates a value that the following logic uses.
                 std::string phone = "",
// This creates or updates a value that the following logic uses.
                 std::string email = "")
// This is part of the current expression or block; read it together with the nearby lines.
        : Account(std::move(username), std::move(fullName), std::move(credential)),
// This is part of the current expression or block; read it together with the nearby lines.
          org_type_(std::move(orgType)),
// This is part of the current expression or block; read it together with the nearby lines.
          approval_status_(std::move(approvalStatus)),
// This is part of the current expression or block; read it together with the nearby lines.
          city_(std::move(city)),
// This is part of the current expression or block; read it together with the nearby lines.
          contactPhone_(std::move(phone)),
// This is part of the current expression or block; read it together with the nearby lines.
          contactEmail_(std::move(email)) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string role() const override { return "ngo"; }

// This creates or updates a value that the following logic uses.
    const std::string& approvalStatus() const { return approval_status_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool isApproved() const { return approval_status_ == "Approved"; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setApprovalStatus(const std::string& status) { approval_status_ = status; }

// This creates or updates a value that the following logic uses.
    const std::string& city() const { return city_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setCity(const std::string& city) { city_ = city; }

// This creates or updates a value that the following logic uses.
    const std::string& contactPhone() const { return contactPhone_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setContactPhone(const std::string& phone) { contactPhone_ = phone; }

// This creates or updates a value that the following logic uses.
    const std::string& contactEmail() const { return contactEmail_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setContactEmail(const std::string& email) { contactEmail_ = email; }

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
            {"dashboardKind", dashboardKind()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"orgType", org_type_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"approvalStatus", approval_status_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"city", city_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"contactPhone", contactPhone_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"contactEmail", contactEmail_}
// This closes the block opened above, such as a function, class, or control-flow branch.
        };
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// OOP: INHERITANCE — Hospital specializes Organization
// This begins a reusable type or template: a blueprint for related data and behavior.
class Hospital final : public Organization {
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    Hospital(std::string username, std::string fullName, Credential credential,
// This creates or updates a value that the following logic uses.
             std::string approvalStatus = "Approved", std::string city = "Nainital",
// This creates or updates a value that the following logic uses.
             std::string phone = "", std::string email = "")
// This is part of the current expression or block; read it together with the nearby lines.
        : Organization(std::move(username), std::move(fullName),
// This is part of the current expression or block; read it together with the nearby lines.
                       std::move(credential), "hospital",
// This is part of the current expression or block; read it together with the nearby lines.
                       std::move(approvalStatus), std::move(city),
// This is part of the current expression or block; read it together with the nearby lines.
                       std::move(phone), std::move(email)) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string dashboardKind() const override { return "ngo"; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string displayName() const override {
// This sends a result back to the code that called this function.
        return fullName() + " (Hospital)";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// OOP: INHERITANCE — NGO specializes Organization
// This begins a reusable type or template: a blueprint for related data and behavior.
class NGO final : public Organization {
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    NGO(std::string username, std::string fullName, Credential credential,
// This creates or updates a value that the following logic uses.
        std::string approvalStatus = "Approved", std::string city = "Nainital",
// This creates or updates a value that the following logic uses.
        std::string phone = "", std::string email = "")
// This is part of the current expression or block; read it together with the nearby lines.
        : Organization(std::move(username), std::move(fullName),
// This is part of the current expression or block; read it together with the nearby lines.
                       std::move(credential), "ngo",
// This is part of the current expression or block; read it together with the nearby lines.
                       std::move(approvalStatus), std::move(city),
// This is part of the current expression or block; read it together with the nearby lines.
                       std::move(phone), std::move(email)) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string dashboardKind() const override { return "ngo"; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string displayName() const override {
// This sends a result back to the code that called this function.
        return fullName() + " (NGO)";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
