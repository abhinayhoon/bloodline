#pragma once
#include "bloodline/Errors.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <algorithm>

namespace bloodline {

enum class CampStatus {
    Upcoming,
    Open,
    Full,
    Completed,
    Cancelled
};

inline std::string campStatusToString(CampStatus s) {
    switch (s) {
        case CampStatus::Upcoming: return "Upcoming";
        case CampStatus::Open: return "Open";
        case CampStatus::Full: return "Full";
        case CampStatus::Completed: return "Completed";
        case CampStatus::Cancelled: return "Cancelled";
    }
    return "Upcoming";
}

inline CampStatus campStatusFromString(const std::string& s) {
    if (s == "Open") return CampStatus::Open;
    if (s == "Full") return CampStatus::Full;
    if (s == "Completed") return CampStatus::Completed;
    if (s == "Cancelled") return CampStatus::Cancelled;
    return CampStatus::Upcoming;
}

// Camp tracks a blood donation drive.
// Encapsulation: Valid status transitions are strictly enforced and throw on illegal transitions.
class Camp {
    std::string id_;
    std::string name_;
    std::string date_;
    int target_units_;
    int units_collected_ = 0;
    int deferred_count_ = 0;
    int slot_capacity_ = 100;
    CampStatus status_ = CampStatus::Upcoming;
    std::vector<std::string> registered_donors_;

public:
    Camp(std::string id, std::string name, std::string date, int targetUnits,
         int slotCapacity = 0, CampStatus status = CampStatus::Upcoming,
         int unitsCollected = 0, int deferredCount = 0)
        : id_(std::move(id)),
          name_(std::move(name)),
          date_(std::move(date)),
          target_units_(targetUnits),
          units_collected_(unitsCollected),
          deferred_count_(deferredCount),
          slot_capacity_(slotCapacity > 0 ? slotCapacity : targetUnits * 2),
          status_(status) {}

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& date() const { return date_; }
    int targetUnits() const { return target_units_; }
    int unitsCollected() const { return units_collected_; }
    int deferredCount() const { return deferred_count_; }
    int slotCapacity() const { return slot_capacity_; }
    CampStatus status() const { return status_; }
    std::string statusString() const { return campStatusToString(status_); }
    const std::vector<std::string>& registeredDonors() const { return registered_donors_; }

    int registeredCount() const { return static_cast<int>(registered_donors_.size()); }
    int remainingSlots() const {
        int rem = slot_capacity_ - registeredCount();
        return rem > 0 ? rem : 0;
    }

    void addUnit() {
        ++units_collected_;
        if (units_collected_ >= target_units_ && status_ == CampStatus::Open) {
            status_ = CampStatus::Full;
        }
    }

    void addDeferred() {
        ++deferred_count_;
    }

    // Encapsulation: State machine validation
    void setStatus(CampStatus newStatus) {
        if (status_ == newStatus) return;

        if (status_ == CampStatus::Cancelled) {
            throw InvalidStateTransitionError("Cancelled camp cannot change status");
        }
        if (status_ == CampStatus::Completed) {
            throw InvalidStateTransitionError("Completed camp cannot change status");
        }

        if (newStatus == CampStatus::Full && status_ != CampStatus::Open && status_ != CampStatus::Upcoming) {
            throw InvalidStateTransitionError("Only upcoming or open camps can become full");
        }
        if (newStatus == CampStatus::Completed && (status_ != CampStatus::Open && status_ != CampStatus::Full)) {
            throw InvalidStateTransitionError("Only open or full camps can be completed");
        }

        status_ = newStatus;
    }

    void registerDonor(const std::string& donorUsername) {
        if (status_ == CampStatus::Cancelled) {
            throw BadRequestError("Cannot register for a cancelled camp");
        }
        if (status_ == CampStatus::Completed) {
            throw BadRequestError("Cannot register for a completed camp");
        }
        if (std::find(registered_donors_.begin(), registered_donors_.end(), donorUsername) != registered_donors_.end()) {
            throw ConflictError("You are already registered for this camp");
        }
        if (status_ == CampStatus::Full || remainingSlots() <= 0) {
            throw ConflictError("Camp registration slots are full");
        }
        registered_donors_.push_back(donorUsername);
        if (remainingSlots() <= 0 && status_ == CampStatus::Upcoming) {
            status_ = CampStatus::Full;
        }
    }

    void unregisterDonor(const std::string& donorUsername) {
        if (status_ == CampStatus::Cancelled || status_ == CampStatus::Completed) {
            throw BadRequestError("Cannot unregister from a completed or cancelled camp");
        }
        auto it = std::find(registered_donors_.begin(), registered_donors_.end(), donorUsername);
        if (it == registered_donors_.end()) {
            throw NotFoundError("You are not registered for this camp");
        }
        registered_donors_.erase(it);
        if (status_ == CampStatus::Full && remainingSlots() > 0) {
            status_ = CampStatus::Upcoming;
        }
    }

    void addRegisteredDonorDirect(const std::string& donorUsername) {
        if (std::find(registered_donors_.begin(), registered_donors_.end(), donorUsername) == registered_donors_.end()) {
            registered_donors_.push_back(donorUsername);
        }
    }

    void editDetails(const std::string& newDate, int newTarget) {
        if (status_ == CampStatus::Completed || status_ == CampStatus::Cancelled) {
            throw BadRequestError("Cannot edit completed or cancelled camps");
        }
        if (newTarget <= 0) {
            throw BadRequestError("Target units must be positive");
        }
        date_ = newDate;
        target_units_ = newTarget;
        if (slot_capacity_ < newTarget) {
            slot_capacity_ = newTarget * 2;
        }
    }

    double progressPercent() const {
        if (target_units_ <= 0) return 0.0;
        double pct = (static_cast<double>(units_collected_) / target_units_) * 100.0;
        return pct > 100.0 ? 100.0 : pct;
    }

    nlohmann::json toJson() const {
        return {
            {"id", id_},
            {"name", name_},
            {"date", date_},
            {"targetUnits", target_units_},
            {"unitsCollected", units_collected_},
            {"deferredCount", deferred_count_},
            {"slotCapacity", slot_capacity_},
            {"registeredCount", registeredCount()},
            {"remainingSlots", remainingSlots()},
            {"status", statusString()},
            {"progressPercent", static_cast<int>(progressPercent())},
            {"registeredDonors", registered_donors_}
        };
    }

    bool operator==(const Camp& other) const { return id_ == other.id_; }
    bool operator<(const Camp& other) const { return date_ < other.date_; }
};

} // namespace bloodline
