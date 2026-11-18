#pragma once
#include "bloodline/Account.hpp"
#include "bloodline/Donor.hpp"
#include "bloodline/Organization.hpp"
#include "bloodline/Errors.hpp"
#include <memory>
#include <string>

namespace bloodline {

// Design Pattern: Factory Method for creating concrete Account instances by role.
class AccountFactory {
public:
    static std::unique_ptr<Account> createAccount(
        const std::string& role,
        const std::string& username,
        const std::string& fullName,
        Credential credential,
        const std::string& orgType = "ngo",
        const std::string& approvalStatus = "Pending",
        const std::string& city = "Nainital")
    {
        if (role == "donor") {
            return std::make_unique<Donor>(username, fullName, std::move(credential));
        } else if (role == "admin") {
            return std::make_unique<Admin>(username, fullName, std::move(credential));
        } else if (role == "ngo") {
            if (orgType == "hospital") {
                return std::make_unique<Hospital>(username, fullName, std::move(credential), approvalStatus, city);
            } else {
                return std::make_unique<NGO>(username, fullName, std::move(credential), approvalStatus, city);
            }
        }
        throw BadRequestError("Invalid account role: " + role);
    }
};

} // namespace bloodline
