// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/Donation.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in <nlohmann/json.hpp>, so this file can use its declarations.
#include <nlohmann/json.hpp>
// This brings in <string>, so this file can use its declarations.
#include <string>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// A Donation links a donor to a camp on a date with a status.
// This is a source comment that explains nearby code.
// Deferred donations don't count toward camp unit totals.

// This begins a reusable type or template: a blueprint for related data and behavior.
class Donation {
// This creates or updates a value that the following logic uses.
    std::string id_;
// This creates or updates a value that the following logic uses.
    std::string donor_username_;
// This creates or updates a value that the following logic uses.
    std::string donor_name_;
// This creates or updates a value that the following logic uses.
    std::string blood_group_;
// This creates or updates a value that the following logic uses.
    std::string camp_id_;
// This creates or updates a value that the following logic uses.
    std::string camp_name_;
// This creates or updates a value that the following logic uses.
    std::string date_;
// This creates or updates a value that the following logic uses.
    std::string status_; // "Completed" or "Deferred"

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    Donation(std::string id, std::string donorUsername, std::string donorName,
// This creates or updates a value that the following logic uses.
             std::string bloodGroup, std::string campId, std::string campName,
// This creates or updates a value that the following logic uses.
             std::string date, std::string status)
// This is part of the current expression or block; read it together with the nearby lines.
        : id_(std::move(id)),
// This is part of the current expression or block; read it together with the nearby lines.
          donor_username_(std::move(donorUsername)),
// This is part of the current expression or block; read it together with the nearby lines.
          donor_name_(std::move(donorName)),
// This is part of the current expression or block; read it together with the nearby lines.
          blood_group_(std::move(bloodGroup)),
// This is part of the current expression or block; read it together with the nearby lines.
          camp_id_(std::move(campId)),
// This is part of the current expression or block; read it together with the nearby lines.
          camp_name_(std::move(campName)),
// This is part of the current expression or block; read it together with the nearby lines.
          date_(std::move(date)),
// This is part of the current expression or block; read it together with the nearby lines.
          status_(std::move(status)) {}

// This creates or updates a value that the following logic uses.
    const std::string& id() const { return id_; }
// This creates or updates a value that the following logic uses.
    const std::string& donorUsername() const { return donor_username_; }
// This creates or updates a value that the following logic uses.
    const std::string& donorName() const { return donor_name_; }
// This creates or updates a value that the following logic uses.
    const std::string& bloodGroup() const { return blood_group_; }
// This creates or updates a value that the following logic uses.
    const std::string& campId() const { return camp_id_; }
// This creates or updates a value that the following logic uses.
    const std::string& campName() const { return camp_name_; }
// This creates or updates a value that the following logic uses.
    const std::string& date() const { return date_; }
// This creates or updates a value that the following logic uses.
    const std::string& status() const { return status_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setStatus(std::string status) { status_ = std::move(status); }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool isCompleted() const { return status_ == "Completed"; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    nlohmann::json toJson() const {
// This sends a result back to the code that called this function.
        return {
// This is part of the current expression or block; read it together with the nearby lines.
            {"id", id_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"donorUsername", donor_username_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"donorName", donor_name_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"bloodGroup", blood_group_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"campId", camp_id_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"campName", camp_name_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"date", date_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"status", status_}
// This closes the block opened above, such as a function, class, or control-flow branch.
        };
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // OOP: POLYMORPHISM (function overloading) — record by username or by name
// This creates or updates a value that the following logic uses.
    static Donation record(const std::string& id,
// This creates or updates a value that the following logic uses.
                           const std::string& donorUsername,
// This creates or updates a value that the following logic uses.
                           const std::string& donorName,
// This creates or updates a value that the following logic uses.
                           const std::string& bloodGroup,
// This creates or updates a value that the following logic uses.
                           const std::string& campId,
// This creates or updates a value that the following logic uses.
                           const std::string& campName,
// This creates or updates a value that the following logic uses.
                           const std::string& date,
// This creates or updates a value that the following logic uses.
                           const std::string& status) {
// This sends a result back to the code that called this function.
        return Donation(id, donorUsername, donorName, bloodGroup,
// This performs one step of the current operation, then continues to the next statement.
                        campId, campName, date, status);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Overloaded: record by Donor reference (via username + name from donor)
// This begins a reusable type or template: a blueprint for related data and behavior.
    template <typename DonorType>
// This creates or updates a value that the following logic uses.
    static Donation record(const std::string& id,
// This creates or updates a value that the following logic uses.
                           const DonorType& donor,
// This creates or updates a value that the following logic uses.
                           const std::string& campId,
// This creates or updates a value that the following logic uses.
                           const std::string& campName,
// This creates or updates a value that the following logic uses.
                           const std::string& date,
// This creates or updates a value that the following logic uses.
                           const std::string& status) {
// This sends a result back to the code that called this function.
        return Donation(id, donor.username(), donor.fullName(),
// This performs one step of the current operation, then continues to the next statement.
                        donor.bloodGroupString(), campId, campName, date, status);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
