// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/Compatibility.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in "bloodline/BloodGroup.hpp", so this file can use its declarations.
#include "bloodline/BloodGroup.hpp"
// This brings in <nlohmann/json.hpp>, so this file can use its declarations.
#include <nlohmann/json.hpp>
// This brings in <array>, so this file can use its declarations.
#include <array>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// CompatibilityTable encodes standard ABO/Rh red blood cell donation rules.
// This is a source comment that explains nearby code.
// The matrix is indexed by BloodGroup enum values (0-7).

// This begins a reusable type or template: a blueprint for related data and behavior.
class CompatibilityTable {
// This is a source comment that explains nearby code.
    // compatibility_[donor][recipient] = true if donor can give to recipient
// This creates or updates a value that the following logic uses.
    static constexpr int N = 8;
// This creates or updates a value that the following logic uses.
    std::array<std::array<bool, N>, N> matrix_{};

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    static int idx(BloodGroup bg) { return static_cast<int>(bg); }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void set(BloodGroup donor, BloodGroup recipient) {
// This performs one step of the current operation, then continues to the next statement.
        matrix_[idx(donor)][idx(recipient)] = true;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// The following members are available to other parts of the program.
public:
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    CompatibilityTable() {
// This is a source comment that explains nearby code.
        // O- can donate to everyone
// This controls which part of the program runs next, based on the current condition or result.
        for (auto bg : kAllBloodGroups) set(BloodGroup::ONeg, bg);

// This is a source comment that explains nearby code.
        // O+ can donate to all positive types
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::OPos, BloodGroup::APos);
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::OPos, BloodGroup::BPos);
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::OPos, BloodGroup::ABPos);
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::OPos, BloodGroup::OPos);

// This is a source comment that explains nearby code.
        // A- can donate to A+, A-, AB+, AB-
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::ANeg, BloodGroup::APos);
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::ANeg, BloodGroup::ANeg);
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::ANeg, BloodGroup::ABPos);
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::ANeg, BloodGroup::ABNeg);

// This is a source comment that explains nearby code.
        // A+ can donate to A+, AB+
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::APos, BloodGroup::APos);
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::APos, BloodGroup::ABPos);

// This is a source comment that explains nearby code.
        // B- can donate to B+, B-, AB+, AB-
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::BNeg, BloodGroup::BPos);
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::BNeg, BloodGroup::BNeg);
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::BNeg, BloodGroup::ABPos);
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::BNeg, BloodGroup::ABNeg);

// This is a source comment that explains nearby code.
        // B+ can donate to B+, AB+
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::BPos, BloodGroup::BPos);
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::BPos, BloodGroup::ABPos);

// This is a source comment that explains nearby code.
        // AB- can donate to AB+, AB-
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::ABNeg, BloodGroup::ABPos);
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::ABNeg, BloodGroup::ABNeg);

// This is a source comment that explains nearby code.
        // AB+ can donate only to AB+
// This performs one step of the current operation, then continues to the next statement.
        set(BloodGroup::ABPos, BloodGroup::ABPos);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool canDonateTo(BloodGroup donor, BloodGroup recipient) const {
// This sends a result back to the code that called this function.
        return matrix_[idx(donor)][idx(recipient)];
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool canReceiveFrom(BloodGroup recipient, BloodGroup donor) const {
// This sends a result back to the code that called this function.
        return canDonateTo(donor, recipient);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    nlohmann::json toJson() const {
// This creates or updates a value that the following logic uses.
        nlohmann::json result = nlohmann::json::array();
// This controls which part of the program runs next, based on the current condition or result.
        for (auto donor : kAllBloodGroups) {
// This creates or updates a value that the following logic uses.
            nlohmann::json entry;
// This performs one step of the current operation, then continues to the next statement.
            entry["type"] = bloodGroupToString(donor);
// This creates or updates a value that the following logic uses.
            nlohmann::json donateTo = nlohmann::json::array();
// This creates or updates a value that the following logic uses.
            nlohmann::json receiveFrom = nlohmann::json::array();
// This controls which part of the program runs next, based on the current condition or result.
            for (auto other : kAllBloodGroups) {
// This controls which part of the program runs next, based on the current condition or result.
                if (canDonateTo(donor, other))
// This performs one step of the current operation, then continues to the next statement.
                    donateTo.push_back(bloodGroupToString(other));
// This controls which part of the program runs next, based on the current condition or result.
                if (canReceiveFrom(donor, other))
// This performs one step of the current operation, then continues to the next statement.
                    receiveFrom.push_back(bloodGroupToString(other));
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This performs one step of the current operation, then continues to the next statement.
            entry["donateTo"] = donateTo;
// This performs one step of the current operation, then continues to the next statement.
            entry["receiveFrom"] = receiveFrom;
// This performs one step of the current operation, then continues to the next statement.
            result.push_back(entry);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return result;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
