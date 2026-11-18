#pragma once
#include <string>
#include <optional>

namespace bloodline {

// OOP: ABSTRACTION — IEligibilityRule is a pure interface.
// The eligibility engine knows only this interface. Concrete rules
// (AgeRule, WeightRule, HemoglobinRule) are hidden behind it.

struct EligibilityResult {
    std::string label;
    std::string status;  // "pass", "fail", or "pending"
    std::string message;
};

class IEligibilityRule {
public:
    virtual ~IEligibilityRule() = default;
    virtual EligibilityResult check(int age, const std::string& gender,
                                    double weight,
                                    std::optional<double> hemoglobin) const = 0;
};

} // namespace bloodline
