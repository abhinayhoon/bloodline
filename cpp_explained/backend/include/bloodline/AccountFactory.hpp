// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/AccountFactory.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in "bloodline/Account.hpp", so this file can use its declarations.
#include "bloodline/Account.hpp"
// This brings in "bloodline/Donor.hpp", so this file can use its declarations.
#include "bloodline/Donor.hpp"
// This brings in "bloodline/Organization.hpp", so this file can use its declarations.
#include "bloodline/Organization.hpp"
// This brings in "bloodline/Errors.hpp", so this file can use its declarations.
#include "bloodline/Errors.hpp"
// This brings in <memory>, so this file can use its declarations.
#include <memory>
// This brings in <string>, so this file can use its declarations.
#include <string>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// Design Pattern: Factory Method for creating concrete Account instances by role.
// This begins a reusable type or template: a blueprint for related data and behavior.
class AccountFactory {
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    static std::unique_ptr<Account> createAccount(
// This creates or updates a value that the following logic uses.
        const std::string& role,
// This creates or updates a value that the following logic uses.
        const std::string& username,
// This creates or updates a value that the following logic uses.
        const std::string& fullName,
// This is part of the current expression or block; read it together with the nearby lines.
        Credential credential,
// This creates or updates a value that the following logic uses.
        const std::string& orgType = "ngo",
// This creates or updates a value that the following logic uses.
        const std::string& approvalStatus = "Pending",
// This creates or updates a value that the following logic uses.
        const std::string& city = "Nainital")
// This opens a scope for the related statements that follow.
    {
// This controls which part of the program runs next, based on the current condition or result.
        if (role == "donor") {
// This sends a result back to the code that called this function.
            return std::make_unique<Donor>(username, fullName, std::move(credential));
// This closes the block opened above, such as a function, class, or control-flow branch.
        } else if (role == "admin") {
// This sends a result back to the code that called this function.
            return std::make_unique<Admin>(username, fullName, std::move(credential));
// This closes the block opened above, such as a function, class, or control-flow branch.
        } else if (role == "ngo") {
// This controls which part of the program runs next, based on the current condition or result.
            if (orgType == "hospital") {
// This sends a result back to the code that called this function.
                return std::make_unique<Hospital>(username, fullName, std::move(credential), approvalStatus, city);
// This closes the block opened above, such as a function, class, or control-flow branch.
            } else {
// This sends a result back to the code that called this function.
                return std::make_unique<NGO>(username, fullName, std::move(credential), approvalStatus, city);
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This reports an error so the caller can handle the failed operation.
        throw BadRequestError("Invalid account role: " + role);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
