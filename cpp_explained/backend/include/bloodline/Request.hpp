// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/Request.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in "bloodline/BloodGroup.hpp", so this file can use its declarations.
#include "bloodline/BloodGroup.hpp"
// This brings in "bloodline/Errors.hpp", so this file can use its declarations.
#include "bloodline/Errors.hpp"
// This brings in "bloodline/DateUtil.hpp", so this file can use its declarations.
#include "bloodline/DateUtil.hpp"
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <vector>, so this file can use its declarations.
#include <vector>
// This brings in <algorithm>, so this file can use its declarations.
#include <algorithm>
// This brings in <memory>, so this file can use its declarations.
#include <memory>
// This brings in <nlohmann/json.hpp>, so this file can use its declarations.
#include <nlohmann/json.hpp>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This begins a reusable type or template: a blueprint for related data and behavior.
enum class RequestStatus {
// This is part of the current expression or block; read it together with the nearby lines.
    Open,
// This is part of the current expression or block; read it together with the nearby lines.
    Fulfilled,
// This is part of the current expression or block; read it together with the nearby lines.
    Closed
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
inline std::string requestStatusToString(RequestStatus s) {
// This controls which part of the program runs next, based on the current condition or result.
    switch (s) {
// This controls which part of the program runs next, based on the current condition or result.
        case RequestStatus::Open: return "Open";
// This controls which part of the program runs next, based on the current condition or result.
        case RequestStatus::Fulfilled: return "Fulfilled";
// This controls which part of the program runs next, based on the current condition or result.
        case RequestStatus::Closed: return "Closed";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This sends a result back to the code that called this function.
    return "Open";
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
inline RequestStatus requestStatusFromString(const std::string& s) {
// This controls which part of the program runs next, based on the current condition or result.
    if (s == "Fulfilled") return RequestStatus::Fulfilled;
// This controls which part of the program runs next, based on the current condition or result.
    if (s == "Closed") return RequestStatus::Closed;
// This sends a result back to the code that called this function.
    return RequestStatus::Open;
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// Inheritance: Abstract base class representing blood requests.
// This begins a reusable type or template: a blueprint for related data and behavior.
class Request {
// The following members are shared with this class and its child classes.
protected:
// This creates or updates a value that the following logic uses.
    std::string id_;
// This creates or updates a value that the following logic uses.
    std::string orgUsername_;
// This creates or updates a value that the following logic uses.
    std::string hospitalName_;
// This performs one step of the current operation, then continues to the next statement.
    BloodGroup bloodGroup_;
// This creates or updates a value that the following logic uses.
    int unitsNeeded_;
// This creates or updates a value that the following logic uses.
    int unitsPledged_;
// This performs one step of the current operation, then continues to the next statement.
    RequestStatus status_;
// This creates or updates a value that the following logic uses.
    std::string createdAt_;
// This creates or updates a value that the following logic uses.
    std::vector<std::string> responders_; // Donor usernames who volunteered

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    Request(const std::string& id, const std::string& orgUsername,
// This creates or updates a value that the following logic uses.
            const std::string& hospitalName, BloodGroup bg, int unitsNeeded,
// This creates or updates a value that the following logic uses.
            const std::string& createdAt = "", RequestStatus status = RequestStatus::Open)
// This is part of the current expression or block; read it together with the nearby lines.
        : id_(id), orgUsername_(orgUsername), hospitalName_(hospitalName),
// This is part of the current expression or block; read it together with the nearby lines.
          bloodGroup_(bg), unitsNeeded_(unitsNeeded), unitsPledged_(0),
// This is part of the current expression or block; read it together with the nearby lines.
          status_(status),
// This is part of the current expression or block; read it together with the nearby lines.
          createdAt_(createdAt.empty() ? DateUtil::today() : createdAt) {}

// This performs one step of the current operation, then continues to the next statement.
    virtual ~Request() = default;

// This creates or updates a value that the following logic uses.
    const std::string& id() const noexcept { return id_; }
// This creates or updates a value that the following logic uses.
    const std::string& orgUsername() const noexcept { return orgUsername_; }
// This creates or updates a value that the following logic uses.
    const std::string& hospitalName() const noexcept { return hospitalName_; }
// This creates or updates a value that the following logic uses.
    BloodGroup bloodGroup() const noexcept { return bloodGroup_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string bloodGroupString() const { return bloodGroupToString(bloodGroup_); }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int unitsNeeded() const noexcept { return unitsNeeded_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int unitsPledged() const noexcept { return unitsPledged_; }
// This creates or updates a value that the following logic uses.
    RequestStatus status() const noexcept { return status_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string statusString() const { return requestStatusToString(status_); }
// This creates or updates a value that the following logic uses.
    const std::string& createdAt() const noexcept { return createdAt_; }
// This creates or updates a value that the following logic uses.
    const std::vector<std::string>& responders() const noexcept { return responders_; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    virtual std::string urgency() const = 0;
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    virtual int priorityScore() const = 0; // Pure virtual: dynamically dispatched

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void addResponder(const std::string& donorUsername) {
// This controls which part of the program runs next, based on the current condition or result.
        if (status_ != RequestStatus::Open) {
// This reports an error so the caller can handle the failed operation.
            throw InvalidStateTransitionError("Cannot pledge to a non-open request");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This controls which part of the program runs next, based on the current condition or result.
        if (std::find(responders_.begin(), responders_.end(), donorUsername) == responders_.end()) {
// This performs one step of the current operation, then continues to the next statement.
            responders_.push_back(donorUsername);
// This performs one step of the current operation, then continues to the next statement.
            unitsPledged_++;
// This is a source comment that explains nearby code.
            // A pledge is an intention, not a collected unit. Organizations confirm fulfillment.
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Persistence restoration must preserve the saved lifecycle state.
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void restoreResponder(const std::string& username) {
// This controls which part of the program runs next, based on the current condition or result.
        if (std::find(responders_.begin(), responders_.end(), username) == responders_.end()) {
// This performs one step of the current operation, then continues to the next statement.
            responders_.push_back(username);
// This creates or updates a value that the following logic uses.
            unitsPledged_ = static_cast<int>(responders_.size());
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Encapsulation: Valid status transitions
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void markFulfilled() {
// This controls which part of the program runs next, based on the current condition or result.
        if (status_ == RequestStatus::Closed) {
// This reports an error so the caller can handle the failed operation.
            throw InvalidStateTransitionError("Closed request cannot be marked fulfilled");
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        status_ = RequestStatus::Fulfilled;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void markClosed() {
// This performs one step of the current operation, then continues to the next statement.
        status_ = RequestStatus::Closed;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    virtual nlohmann::json toJson() const {
// This sends a result back to the code that called this function.
        return {
// This is part of the current expression or block; read it together with the nearby lines.
            {"id", id_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"orgUsername", orgUsername_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"hospitalName", hospitalName_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"bloodGroup", bloodGroupString()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"unitsNeeded", unitsNeeded_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"unitsPledged", unitsPledged_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"status", statusString()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"urgency", urgency()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"priorityScore", priorityScore()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"createdAt", createdAt_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"responderCount", responders_.size()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"responders", responders_}
// This closes the block opened above, such as a function, class, or control-flow branch.
        };
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// Subclass: UrgentRequest (Urgency: "Urgent" or "Critical")
// This begins a reusable type or template: a blueprint for related data and behavior.
class UrgentRequest : public Request {
// This creates or updates a value that the following logic uses.
    bool isCritical_;
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    UrgentRequest(const std::string& id, const std::string& orgUsername,
// This creates or updates a value that the following logic uses.
                  const std::string& hospitalName, BloodGroup bg, int unitsNeeded,
// This creates or updates a value that the following logic uses.
                  bool isCritical = false, const std::string& createdAt = "",
// This is part of the current expression or block; read it together with the nearby lines.
                  RequestStatus status = RequestStatus::Open)
// This is part of the current expression or block; read it together with the nearby lines.
        : Request(id, orgUsername, hospitalName, bg, unitsNeeded, createdAt, status),
// This is part of the current expression or block; read it together with the nearby lines.
          isCritical_(isCritical) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string urgency() const override {
// This sends a result back to the code that called this function.
        return isCritical_ ? "Critical" : "Urgent";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int priorityScore() const override {
// This sends a result back to the code that called this function.
        return isCritical_ ? 100 : 75;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// Subclass: RoutineRequest (Urgency: "Normal")
// This begins a reusable type or template: a blueprint for related data and behavior.
class RoutineRequest : public Request {
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    RoutineRequest(const std::string& id, const std::string& orgUsername,
// This creates or updates a value that the following logic uses.
                   const std::string& hospitalName, BloodGroup bg, int unitsNeeded,
// This creates or updates a value that the following logic uses.
                   const std::string& createdAt = "",
// This is part of the current expression or block; read it together with the nearby lines.
                   RequestStatus status = RequestStatus::Open)
// This is part of the current expression or block; read it together with the nearby lines.
        : Request(id, orgUsername, hospitalName, bg, unitsNeeded, createdAt, status) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string urgency() const override {
// This sends a result back to the code that called this function.
        return "Normal";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int priorityScore() const override {
// This sends a result back to the code that called this function.
        return 25;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// Polymorphism: RequestPrioritizer sorts requests via virtual dispatch
// This begins a reusable type or template: a blueprint for related data and behavior.
class RequestPrioritizer {
// The following members are available to other parts of the program.
public:
// This begins a reusable type or template: a blueprint for related data and behavior.
    template <typename Container>
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    static void sort(Container& requests) {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::sort(requests.begin(), requests.end(), [](const auto& a, const auto& b) {
// This is a source comment that explains nearby code.
            // Polymorphic dispatch on priorityScore()
// This controls which part of the program runs next, based on the current condition or result.
            if (a->priorityScore() != b->priorityScore()) {
// This sends a result back to the code that called this function.
                return a->priorityScore() > b->priorityScore();
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This is a source comment that explains nearby code.
            // Tie-break by older creation date first
// This sends a result back to the code that called this function.
            return a->createdAt() < b->createdAt();
// This closes the block opened above, such as a function, class, or control-flow branch.
        });
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
