#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace bloodline {

// A Donation links a donor to a camp on a date with a status.
// Deferred donations don't count toward camp unit totals.

class Donation {
    std::string id_;
    std::string donor_username_;
    std::string donor_name_;
    std::string blood_group_;
    std::string camp_id_;
    std::string camp_name_;
    std::string date_;
    std::string status_; // "Completed" or "Deferred"

public:
    Donation(std::string id, std::string donorUsername, std::string donorName,
             std::string bloodGroup, std::string campId, std::string campName,
             std::string date, std::string status)
        : id_(std::move(id)),
          donor_username_(std::move(donorUsername)),
          donor_name_(std::move(donorName)),
          blood_group_(std::move(bloodGroup)),
          camp_id_(std::move(campId)),
          camp_name_(std::move(campName)),
          date_(std::move(date)),
          status_(std::move(status)) {}

    const std::string& id() const { return id_; }
    const std::string& donorUsername() const { return donor_username_; }
    const std::string& donorName() const { return donor_name_; }
    const std::string& bloodGroup() const { return blood_group_; }
    const std::string& campId() const { return camp_id_; }
    const std::string& campName() const { return camp_name_; }
    const std::string& date() const { return date_; }
    const std::string& status() const { return status_; }
    void setStatus(std::string status) { status_ = std::move(status); }

    bool isCompleted() const { return status_ == "Completed"; }

    nlohmann::json toJson() const {
        return {
            {"id", id_},
            {"donorUsername", donor_username_},
            {"donorName", donor_name_},
            {"bloodGroup", blood_group_},
            {"campId", camp_id_},
            {"campName", camp_name_},
            {"date", date_},
            {"status", status_}
        };
    }

    // OOP: POLYMORPHISM (function overloading) — record by username or by name
    static Donation record(const std::string& id,
                           const std::string& donorUsername,
                           const std::string& donorName,
                           const std::string& bloodGroup,
                           const std::string& campId,
                           const std::string& campName,
                           const std::string& date,
                           const std::string& status) {
        return Donation(id, donorUsername, donorName, bloodGroup,
                        campId, campName, date, status);
    }

    // Overloaded: record by Donor reference (via username + name from donor)
    template <typename DonorType>
    static Donation record(const std::string& id,
                           const DonorType& donor,
                           const std::string& campId,
                           const std::string& campName,
                           const std::string& date,
                           const std::string& status) {
        return Donation(id, donor.username(), donor.fullName(),
                        donor.bloodGroupString(), campId, campName, date, status);
    }
};

} // namespace bloodline
