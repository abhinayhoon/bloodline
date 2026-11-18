#pragma once
#include "bloodline/BloodGroup.hpp"
#include "bloodline/Errors.hpp"
#include "bloodline/DateUtil.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <nlohmann/json.hpp>

namespace bloodline {

enum class RequestStatus {
    Open,
    Fulfilled,
    Closed
};

inline std::string requestStatusToString(RequestStatus s) {
    switch (s) {
        case RequestStatus::Open: return "Open";
        case RequestStatus::Fulfilled: return "Fulfilled";
        case RequestStatus::Closed: return "Closed";
    }
    return "Open";
}

inline RequestStatus requestStatusFromString(const std::string& s) {
    if (s == "Fulfilled") return RequestStatus::Fulfilled;
    if (s == "Closed") return RequestStatus::Closed;
    return RequestStatus::Open;
}

// Inheritance: Abstract base class representing blood requests.
class Request {
protected:
    std::string id_;
    std::string orgUsername_;
    std::string hospitalName_;
    BloodGroup bloodGroup_;
    int unitsNeeded_;
    int unitsPledged_;
    RequestStatus status_;
    std::string createdAt_;
    std::vector<std::string> responders_; // Donor usernames who volunteered

public:
    Request(const std::string& id, const std::string& orgUsername,
            const std::string& hospitalName, BloodGroup bg, int unitsNeeded,
            const std::string& createdAt = "", RequestStatus status = RequestStatus::Open)
        : id_(id), orgUsername_(orgUsername), hospitalName_(hospitalName),
          bloodGroup_(bg), unitsNeeded_(unitsNeeded), unitsPledged_(0),
          status_(status),
          createdAt_(createdAt.empty() ? DateUtil::today() : createdAt) {}

    virtual ~Request() = default;

    const std::string& id() const noexcept { return id_; }
    const std::string& orgUsername() const noexcept { return orgUsername_; }
    const std::string& hospitalName() const noexcept { return hospitalName_; }
    BloodGroup bloodGroup() const noexcept { return bloodGroup_; }
    std::string bloodGroupString() const { return bloodGroupToString(bloodGroup_); }
    int unitsNeeded() const noexcept { return unitsNeeded_; }
    int unitsPledged() const noexcept { return unitsPledged_; }
    RequestStatus status() const noexcept { return status_; }
    std::string statusString() const { return requestStatusToString(status_); }
    const std::string& createdAt() const noexcept { return createdAt_; }
    const std::vector<std::string>& responders() const noexcept { return responders_; }

    virtual std::string urgency() const = 0;
    virtual int priorityScore() const = 0; // Pure virtual: dynamically dispatched

    void addResponder(const std::string& donorUsername) {
        if (status_ != RequestStatus::Open) {
            throw InvalidStateTransitionError("Cannot pledge to a non-open request");
        }
        if (std::find(responders_.begin(), responders_.end(), donorUsername) == responders_.end()) {
            responders_.push_back(donorUsername);
            unitsPledged_++;
            // A pledge is an intention, not a collected unit. Organizations confirm fulfillment.
        }
    }

    // Persistence restoration must preserve the saved lifecycle state.
    void restoreResponder(const std::string& username) {
        if (std::find(responders_.begin(), responders_.end(), username) == responders_.end()) {
            responders_.push_back(username);
            unitsPledged_ = static_cast<int>(responders_.size());
        }
    }

    // Encapsulation: Valid status transitions
    void markFulfilled() {
        if (status_ == RequestStatus::Closed) {
            throw InvalidStateTransitionError("Closed request cannot be marked fulfilled");
        }
        status_ = RequestStatus::Fulfilled;
    }

    void markClosed() {
        status_ = RequestStatus::Closed;
    }

    virtual nlohmann::json toJson() const {
        return {
            {"id", id_},
            {"orgUsername", orgUsername_},
            {"hospitalName", hospitalName_},
            {"bloodGroup", bloodGroupString()},
            {"unitsNeeded", unitsNeeded_},
            {"unitsPledged", unitsPledged_},
            {"status", statusString()},
            {"urgency", urgency()},
            {"priorityScore", priorityScore()},
            {"createdAt", createdAt_},
            {"responderCount", responders_.size()},
            {"responders", responders_}
        };
    }
};

// Subclass: UrgentRequest (Urgency: "Urgent" or "Critical")
class UrgentRequest : public Request {
    bool isCritical_;
public:
    UrgentRequest(const std::string& id, const std::string& orgUsername,
                  const std::string& hospitalName, BloodGroup bg, int unitsNeeded,
                  bool isCritical = false, const std::string& createdAt = "",
                  RequestStatus status = RequestStatus::Open)
        : Request(id, orgUsername, hospitalName, bg, unitsNeeded, createdAt, status),
          isCritical_(isCritical) {}

    std::string urgency() const override {
        return isCritical_ ? "Critical" : "Urgent";
    }

    int priorityScore() const override {
        return isCritical_ ? 100 : 75;
    }
};

// Subclass: RoutineRequest (Urgency: "Normal")
class RoutineRequest : public Request {
public:
    RoutineRequest(const std::string& id, const std::string& orgUsername,
                   const std::string& hospitalName, BloodGroup bg, int unitsNeeded,
                   const std::string& createdAt = "",
                   RequestStatus status = RequestStatus::Open)
        : Request(id, orgUsername, hospitalName, bg, unitsNeeded, createdAt, status) {}

    std::string urgency() const override {
        return "Normal";
    }

    int priorityScore() const override {
        return 25;
    }
};

// Polymorphism: RequestPrioritizer sorts requests via virtual dispatch
class RequestPrioritizer {
public:
    template <typename Container>
    static void sort(Container& requests) {
        std::sort(requests.begin(), requests.end(), [](const auto& a, const auto& b) {
            // Polymorphic dispatch on priorityScore()
            if (a->priorityScore() != b->priorityScore()) {
                return a->priorityScore() > b->priorityScore();
            }
            // Tie-break by older creation date first
            return a->createdAt() < b->createdAt();
        });
    }
};

} // namespace bloodline
