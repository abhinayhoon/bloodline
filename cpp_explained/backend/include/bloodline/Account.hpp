// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/Account.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in "bloodline/Credential.hpp", so this file can use its declarations.
#include "bloodline/Credential.hpp"
// This brings in <nlohmann/json.hpp>, so this file can use its declarations.
#include <nlohmann/json.hpp>
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <memory>, so this file can use its declarations.
#include <memory>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// OOP: ABSTRACTION & INHERITANCE — Account is the abstract base class.
// This is a source comment that explains nearby code.
// Application code depends on the Account interface (pure virtual methods).
// This is a source comment that explains nearby code.
// Donor, Organization, and Admin derive from it; polymorphic code uses Account*.

// This begins a reusable type or template: a blueprint for related data and behavior.
class Account {
// This creates or updates a value that the following logic uses.
    std::string username_;
// This creates or updates a value that the following logic uses.
    std::string full_name_;
// This performs one step of the current operation, then continues to the next statement.
    Credential credential_;

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    Account(std::string username, std::string fullName, Credential credential)
// This is part of the current expression or block; read it together with the nearby lines.
        : username_(std::move(username)),
// This is part of the current expression or block; read it together with the nearby lines.
          full_name_(std::move(fullName)),
// This is part of the current expression or block; read it together with the nearby lines.
          credential_(std::move(credential)) {}

// This performs one step of the current operation, then continues to the next statement.
    virtual ~Account() = default;

// This is a source comment that explains nearby code.
    // Pure virtual: the kind of dashboard to render for this user
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    virtual std::string dashboardKind() const = 0;

// This is a source comment that explains nearby code.
    // Pure virtual: a human-readable display name
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    virtual std::string displayName() const = 0;

// This is a source comment that explains nearby code.
    // Pure virtual: the role string sent in API responses
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    virtual std::string role() const = 0;

// This is a source comment that explains nearby code.
    // Pure virtual: serialize role-specific fields into a JSON response
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    virtual nlohmann::json toJson() const = 0;

// This creates or updates a value that the following logic uses.
    const std::string& username() const { return username_; }
// This creates or updates a value that the following logic uses.
    const std::string& fullName() const { return full_name_; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setFullName(const std::string& name) { full_name_ = name; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void updateCredential(Credential newCred) { credential_ = std::move(newCred); }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool verifyPassword(const std::string& password) const {
// This sends a result back to the code that called this function.
        return credential_.verify(password);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This creates or updates a value that the following logic uses.
    const Credential& credential() const { return credential_; }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// OOP: INHERITANCE — Admin derives from Account for platform governance
// This begins a reusable type or template: a blueprint for related data and behavior.
class Admin final : public Account {
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    Admin(std::string username, std::string fullName, Credential credential)
// This is part of the current expression or block; read it together with the nearby lines.
        : Account(std::move(username), std::move(fullName), std::move(credential)) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string dashboardKind() const override { return "admin"; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string displayName() const override { return fullName() + " (System Admin)"; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string role() const override { return "admin"; }

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
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
