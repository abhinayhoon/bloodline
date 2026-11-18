#pragma once
#include "bloodline/BloodGroup.hpp"
#include <nlohmann/json.hpp>
#include <array>

namespace bloodline {

// CompatibilityTable encodes standard ABO/Rh red blood cell donation rules.
// The matrix is indexed by BloodGroup enum values (0-7).

class CompatibilityTable {
    // compatibility_[donor][recipient] = true if donor can give to recipient
    static constexpr int N = 8;
    std::array<std::array<bool, N>, N> matrix_{};

    static int idx(BloodGroup bg) { return static_cast<int>(bg); }

    void set(BloodGroup donor, BloodGroup recipient) {
        matrix_[idx(donor)][idx(recipient)] = true;
    }

public:
    CompatibilityTable() {
        // O- can donate to everyone
        for (auto bg : kAllBloodGroups) set(BloodGroup::ONeg, bg);

        // O+ can donate to all positive types
        set(BloodGroup::OPos, BloodGroup::APos);
        set(BloodGroup::OPos, BloodGroup::BPos);
        set(BloodGroup::OPos, BloodGroup::ABPos);
        set(BloodGroup::OPos, BloodGroup::OPos);

        // A- can donate to A+, A-, AB+, AB-
        set(BloodGroup::ANeg, BloodGroup::APos);
        set(BloodGroup::ANeg, BloodGroup::ANeg);
        set(BloodGroup::ANeg, BloodGroup::ABPos);
        set(BloodGroup::ANeg, BloodGroup::ABNeg);

        // A+ can donate to A+, AB+
        set(BloodGroup::APos, BloodGroup::APos);
        set(BloodGroup::APos, BloodGroup::ABPos);

        // B- can donate to B+, B-, AB+, AB-
        set(BloodGroup::BNeg, BloodGroup::BPos);
        set(BloodGroup::BNeg, BloodGroup::BNeg);
        set(BloodGroup::BNeg, BloodGroup::ABPos);
        set(BloodGroup::BNeg, BloodGroup::ABNeg);

        // B+ can donate to B+, AB+
        set(BloodGroup::BPos, BloodGroup::BPos);
        set(BloodGroup::BPos, BloodGroup::ABPos);

        // AB- can donate to AB+, AB-
        set(BloodGroup::ABNeg, BloodGroup::ABPos);
        set(BloodGroup::ABNeg, BloodGroup::ABNeg);

        // AB+ can donate only to AB+
        set(BloodGroup::ABPos, BloodGroup::ABPos);
    }

    bool canDonateTo(BloodGroup donor, BloodGroup recipient) const {
        return matrix_[idx(donor)][idx(recipient)];
    }

    bool canReceiveFrom(BloodGroup recipient, BloodGroup donor) const {
        return canDonateTo(donor, recipient);
    }

    nlohmann::json toJson() const {
        nlohmann::json result = nlohmann::json::array();
        for (auto donor : kAllBloodGroups) {
            nlohmann::json entry;
            entry["type"] = bloodGroupToString(donor);
            nlohmann::json donateTo = nlohmann::json::array();
            nlohmann::json receiveFrom = nlohmann::json::array();
            for (auto other : kAllBloodGroups) {
                if (canDonateTo(donor, other))
                    donateTo.push_back(bloodGroupToString(other));
                if (canReceiveFrom(donor, other))
                    receiveFrom.push_back(bloodGroupToString(other));
            }
            entry["donateTo"] = donateTo;
            entry["receiveFrom"] = receiveFrom;
            result.push_back(entry);
        }
        return result;
    }
};

} // namespace bloodline
