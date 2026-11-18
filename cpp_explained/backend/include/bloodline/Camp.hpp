// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/Camp.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in "bloodline/Errors.hpp", so this file can use its declarations.
#include "bloodline/Errors.hpp"
// This brings in <nlohmann/json.hpp>, so this file can use its declarations.
#include <nlohmann/json.hpp>
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <vector>, so this file can use its declarations.
#include <vector>
// This brings in <algorithm>, so this file can use its declarations.
#include <algorithm>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This begins a reusable type or template: a blueprint for related data and behavior.
enum class CampStatus {
// This is part of the current expression or block; read it together with the nearby lines.
    Upcoming,
// This is part of the current expression or block; read it together with the nearby lines.
    Open,
// This is part of the current expression or block; read it together with the nearby lines.
    Full,
// This is part of the current expression or block; read it together with the nearby lines.
    Completed,
// This is part of the current expression or block; read it together with the nearby lines.
    Cancelled
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
inline std::string campStatusToString(CampStatus s) {
// This controls which part of the program runs next, based on the current condition or result.
    switch (s) {
// This controls which part of the program runs next, based on the current condition or result.
        case CampStatus::Upcoming: return "Upcoming";
// This controls which part of the program runs next, based on the current condition or result.
        case CampStatus::Open: return "Open";
// This controls which part of the program runs next, based on the current condition or result.
        case CampStatus::Full: return "Full";
// This controls which part of the program runs next, based on the current condition or result.
        case CampStatus::Completed: return "Completed";
// This controls which part of the program runs next, based on the current condition or result.
        case CampStatus::Cancelled: return "Cancelled";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This sends a result back to the code that called this function.
    return "Upcoming";
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
inline CampStatus campStatusFromString(const std::string& s) {
// This controls which part of the program runs next, based on the current condition or result.
    if (s == "Open") return CampStatus::Open;
// This controls which part of the program runs next, based on the current condition or result.
    if (s == "Full") return CampStatus::Full;
// This controls which part of the program runs next, based on the current condition or result.
    if (s == "Completed") return CampStatus::Completed;
// This controls which part of the program runs next, based on the current condition or result.
    if (s == "Cancelled") return CampStatus::Cancelled;
// This sends a result back to the code that called this function.
    return CampStatus::Upcoming;
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// Camp tracks a blood donation drive.
// This is a source comment that explains nearby code.
// Encapsulation: Valid status transitions are strictly enforced and throw on illegal transitions.
// This begins a reusable type or template: a blueprint for related data and behavior.
class Camp {
// This creates or updates a value that the following logic uses.
    std::string id_;
// This creates or updates a value that the following logic uses.
    std::string name_;
// This creates or updates a value that the following logic uses.
    std::string date_;
// This creates or updates a value that the following logic uses.
    int target_units_;
// This creates or updates a value that the following logic uses.
    int units_collected_ = 0;
// This creates or updates a value that the following logic uses.
    int deferred_count_ = 0;
// This creates or updates a value that the following logic uses.
    int slot_capacity_ = 100;
// This performs one step of the current operation, then continues to the next statement.
    CampStatus status_ = CampStatus::Upcoming;
// This creates or updates a value that the following logic uses.
    std::vector<std::string> registered_donors_;

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    Camp(std::string id, std::string name, std::string date, int targetUnits,
// This creates or updates a value that the following logic uses.
         int slotCapacity = 0, CampStatus status = CampStatus::Upcoming,
// This creates or updates a value that the following logic uses.
         int unitsCollected = 0, int deferredCount = 0)
// This is part of the current expression or block; read it together with the nearby lines.
        : id_(std::move(id)),
// This is part of the current expression or block; read it together with the nearby lines.
          name_(std::move(name)),
// This is part of the current expression or block; read it together with the nearby lines.
          date_(std::move(date)),
// This is part of the current expression or block; read it together with the nearby lines.
          target_units_(targetUnits),
// This is part of the current expression or block; read it together with the nearby lines.
          units_collected_(unitsCollected),
// This is part of the current expression or block; read it together with the nearby lines.
          deferred_count_(deferredCount),
// This is part of the current expression or block; read it together with the nearby lines.
          slot_capacity_(slotCapacity > 0 ? slotCapacity : targetUnits * 2),
// This is part of the current expression or block; read it together with the nearby lines.
          status_(status) {}

// This creates or updates a value that the following logic uses.
    const std::string& id() const { return id_; }
// This creates or updates a value that the following logic uses.
    const std::string& name() const { return name_; }
// This creates or updates a value that the following logic uses.
    const std::string& date() const { return date_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int targetUnits() const { return target_units_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int unitsCollected() const { return units_collected_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int deferredCount() const { return deferred_count_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int slotCapacity() const { return slot_capacity_; }
// This creates or updates a value that the following logic uses.
    CampStatus status() const { return status_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string statusString() const { return campStatusToString(status_); }
// This creates or updates a value that the following logic uses.
    const std::vector<std::string>& registeredDonors() const { return registered_donors_; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int registeredCount() const { return static_cast<int>(registered_donors_.size()); }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int remainingSlots() const {
// This creates or updates a value that the following logic uses.
        int rem = slot_capacity_ - registeredCount();
// This sends a result back to the code that called this function.
        return rem > 0 ? rem : 0;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void addUnit() {
// This performs one step of the current operation, then continues to the next statement.
        ++units_collected_;
// This controls which part of the program runs next, based on the current condition or result.
        if (units_collected_ >= target_units_ && status_ == CampStatus::Open) {
// This performs one step of the current operation, then continues to the next statement.
            status_ = CampStatus::Full;
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void addDeferred() {
// This performs one step of the current operation, then continues to the next statement.
        ++deferred_count_;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Encapsulation: State machine validation
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void setStatus(CampStatus newStatus) {
// This controls which part of the program runs next, based on the current condition or result.
        if (status_ == newStatus) return;

// This controls which part of the program runs next, based on the current condition or result.
        if (status_ == CampStatus::Cancelled) {
// This reports an error so the caller can handle the failed operation.
            throw InvalidStateTransitionError("Cancelled camp cannot change status");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This controls which part of the program runs next, based on the current condition or result.
        if (status_ == CampStatus::Completed) {
// This reports an error so the caller can handle the failed operation.
            throw InvalidStateTransitionError("Completed camp cannot change status");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }

// This controls which part of the program runs next, based on the current condition or result.
        if (newStatus == CampStatus::Full && status_ != CampStatus::Open && status_ != CampStatus::Upcoming) {
// This reports an error so the caller can handle the failed operation.
            throw InvalidStateTransitionError("Only upcoming or open camps can become full");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This controls which part of the program runs next, based on the current condition or result.
        if (newStatus == CampStatus::Completed && (status_ != CampStatus::Open && status_ != CampStatus::Full)) {
// This reports an error so the caller can handle the failed operation.
            throw InvalidStateTransitionError("Only open or full camps can be completed");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }

// This performs one step of the current operation, then continues to the next statement.
        status_ = newStatus;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void registerDonor(const std::string& donorUsername) {
// This controls which part of the program runs next, based on the current condition or result.
        if (status_ == CampStatus::Cancelled) {
// This reports an error so the caller can handle the failed operation.
            throw BadRequestError("Cannot register for a cancelled camp");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This controls which part of the program runs next, based on the current condition or result.
        if (status_ == CampStatus::Completed) {
// This reports an error so the caller can handle the failed operation.
            throw BadRequestError("Cannot register for a completed camp");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This controls which part of the program runs next, based on the current condition or result.
        if (std::find(registered_donors_.begin(), registered_donors_.end(), donorUsername) != registered_donors_.end()) {
// This reports an error so the caller can handle the failed operation.
            throw ConflictError("You are already registered for this camp");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This controls which part of the program runs next, based on the current condition or result.
        if (status_ == CampStatus::Full || remainingSlots() <= 0) {
// This reports an error so the caller can handle the failed operation.
            throw ConflictError("Camp registration slots are full");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        registered_donors_.push_back(donorUsername);
// This controls which part of the program runs next, based on the current condition or result.
        if (remainingSlots() <= 0 && status_ == CampStatus::Upcoming) {
// This performs one step of the current operation, then continues to the next statement.
            status_ = CampStatus::Full;
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void unregisterDonor(const std::string& donorUsername) {
// This controls which part of the program runs next, based on the current condition or result.
        if (status_ == CampStatus::Cancelled || status_ == CampStatus::Completed) {
// This reports an error so the caller can handle the failed operation.
            throw BadRequestError("Cannot unregister from a completed or cancelled camp");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This creates or updates a value that the following logic uses.
        auto it = std::find(registered_donors_.begin(), registered_donors_.end(), donorUsername);
// This controls which part of the program runs next, based on the current condition or result.
        if (it == registered_donors_.end()) {
// This reports an error so the caller can handle the failed operation.
            throw NotFoundError("You are not registered for this camp");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        registered_donors_.erase(it);
// This controls which part of the program runs next, based on the current condition or result.
        if (status_ == CampStatus::Full && remainingSlots() > 0) {
// This performs one step of the current operation, then continues to the next statement.
            status_ = CampStatus::Upcoming;
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void addRegisteredDonorDirect(const std::string& donorUsername) {
// This controls which part of the program runs next, based on the current condition or result.
        if (std::find(registered_donors_.begin(), registered_donors_.end(), donorUsername) == registered_donors_.end()) {
// This performs one step of the current operation, then continues to the next statement.
            registered_donors_.push_back(donorUsername);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void editDetails(const std::string& newDate, int newTarget) {
// This controls which part of the program runs next, based on the current condition or result.
        if (status_ == CampStatus::Completed || status_ == CampStatus::Cancelled) {
// This reports an error so the caller can handle the failed operation.
            throw BadRequestError("Cannot edit completed or cancelled camps");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This controls which part of the program runs next, based on the current condition or result.
        if (newTarget <= 0) {
// This reports an error so the caller can handle the failed operation.
            throw BadRequestError("Target units must be positive");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        date_ = newDate;
// This performs one step of the current operation, then continues to the next statement.
        target_units_ = newTarget;
// This controls which part of the program runs next, based on the current condition or result.
        if (slot_capacity_ < newTarget) {
// This performs one step of the current operation, then continues to the next statement.
            slot_capacity_ = newTarget * 2;
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    double progressPercent() const {
// This controls which part of the program runs next, based on the current condition or result.
        if (target_units_ <= 0) return 0.0;
// This creates or updates a value that the following logic uses.
        double pct = (static_cast<double>(units_collected_) / target_units_) * 100.0;
// This sends a result back to the code that called this function.
        return pct > 100.0 ? 100.0 : pct;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    nlohmann::json toJson() const {
// This sends a result back to the code that called this function.
        return {
// This is part of the current expression or block; read it together with the nearby lines.
            {"id", id_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"name", name_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"date", date_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"targetUnits", target_units_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"unitsCollected", units_collected_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"deferredCount", deferred_count_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"slotCapacity", slot_capacity_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"registeredCount", registeredCount()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"remainingSlots", remainingSlots()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"status", statusString()},
// This creates or updates a value that the following logic uses.
            {"progressPercent", static_cast<int>(progressPercent())},
// This is part of the current expression or block; read it together with the nearby lines.
            {"registeredDonors", registered_donors_}
// This closes the block opened above, such as a function, class, or control-flow branch.
        };
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This creates or updates a value that the following logic uses.
    bool operator==(const Camp& other) const { return id_ == other.id_; }
// This creates or updates a value that the following logic uses.
    bool operator<(const Camp& other) const { return date_ < other.date_; }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
