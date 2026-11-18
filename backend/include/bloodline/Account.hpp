#pragma once
#include "bloodline/Credential.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <memory>

namespace bloodline {

// OOP: ABSTRACTION & INHERITANCE — Account is the abstract base class.
// Application code depends on the Account interface (pure virtual methods).
// Donor, Organization, and Admin derive from it; polymorphic code uses Account*.

class Account {
    std::string username_;
    std::string full_name_;
    Credential credential_;

public:
    Account(std::string username, std::string fullName, Credential credential)
        : username_(std::move(username)),
          full_name_(std::move(fullName)),
          credential_(std::move(credential)) {}

    virtual ~Account() = default;

    // Pure virtual: the kind of dashboard to render for this user
    virtual std::string dashboardKind() const = 0;

    // Pure virtual: a human-readable display name
    virtual std::string displayName() const = 0;

    // Pure virtual: the role string sent in API responses
    virtual std::string role() const = 0;

    // Pure virtual: serialize role-specific fields into a JSON response
    virtual nlohmann::json toJson() const = 0;

    const std::string& username() const { return username_; }
    const std::string& fullName() const { return full_name_; }

    void setFullName(const std::string& name) { full_name_ = name; }
    void updateCredential(Credential newCred) { credential_ = std::move(newCred); }

    bool verifyPassword(const std::string& password) const {
        return credential_.verify(password);
    }

    const Credential& credential() const { return credential_; }
};

// OOP: INHERITANCE — Admin derives from Account for platform governance
class Admin final : public Account {
public:
    Admin(std::string username, std::string fullName, Credential credential)
        : Account(std::move(username), std::move(fullName), std::move(credential)) {}

    std::string dashboardKind() const override { return "admin"; }
    std::string displayName() const override { return fullName() + " (System Admin)"; }
    std::string role() const override { return "admin"; }

    nlohmann::json toJson() const override {
        return {
            {"username", username()},
            {"fullName", fullName()},
            {"role", role()},
            {"dashboardKind", dashboardKind()}
        };
    }
};

} // namespace bloodline
