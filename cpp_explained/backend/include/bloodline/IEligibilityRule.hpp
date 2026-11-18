// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/IEligibilityRule.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <optional>, so this file can use its declarations.
#include <optional>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// OOP: ABSTRACTION — IEligibilityRule is a pure interface.
// This is a source comment that explains nearby code.
// The eligibility engine knows only this interface. Concrete rules
// This is a source comment that explains nearby code.
// (AgeRule, WeightRule, HemoglobinRule) are hidden behind it.

// This begins a reusable type or template: a blueprint for related data and behavior.
struct EligibilityResult {
// This creates or updates a value that the following logic uses.
    std::string label;
// This creates or updates a value that the following logic uses.
    std::string status;  // "pass", "fail", or "pending"
// This creates or updates a value that the following logic uses.
    std::string message;
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This begins a reusable type or template: a blueprint for related data and behavior.
class IEligibilityRule {
// The following members are available to other parts of the program.
public:
// This performs one step of the current operation, then continues to the next statement.
    virtual ~IEligibilityRule() = default;
// This creates or updates a value that the following logic uses.
    virtual EligibilityResult check(int age, const std::string& gender,
// This creates or updates a value that the following logic uses.
                                    double weight,
// This creates or updates a value that the following logic uses.
                                    std::optional<double> hemoglobin) const = 0;
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
