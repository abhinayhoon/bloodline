// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/MatchingEngine.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in "bloodline/Donor.hpp", so this file can use its declarations.
#include "bloodline/Donor.hpp"
// This brings in "bloodline/Request.hpp", so this file can use its declarations.
#include "bloodline/Request.hpp"
// This brings in "bloodline/Compatibility.hpp", so this file can use its declarations.
#include "bloodline/Compatibility.hpp"
// This brings in <vector>, so this file can use its declarations.
#include <vector>
// This brings in <memory>, so this file can use its declarations.
#include <memory>
// This brings in <string>, so this file can use its declarations.
#include <string>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// Design Pattern: Strategy Pattern for Donor Matching.
// This is a source comment that explains nearby code.
// IMatchingRule provides an extensible interface. New rules can be plugged in
// This is a source comment that explains nearby code.
// without modifying the core DonorMatchingEngine.

// This begins a reusable type or template: a blueprint for related data and behavior.
class IMatchingRule {
// The following members are available to other parts of the program.
public:
// This performs one step of the current operation, then continues to the next statement.
    virtual ~IMatchingRule() = default;
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    virtual std::string ruleName() const = 0;
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    virtual bool evaluate(const Donor& donor, const Request& request) const = 0;
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// Strategy 1: Blood Compatibility Rule using CompatibilityTable
// This begins a reusable type or template: a blueprint for related data and behavior.
class BloodCompatibilityRule : public IMatchingRule {
// This performs one step of the current operation, then continues to the next statement.
    CompatibilityTable table_;
// The following members are available to other parts of the program.
public:
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string ruleName() const override { return "Blood Compatibility"; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool evaluate(const Donor& donor, const Request& request) const override {
// This controls which part of the program runs next, based on the current condition or result.
        if (!donor.bloodGroup()) return false;
// This sends a result back to the code that called this function.
        return table_.canDonateTo(*donor.bloodGroup(), request.bloodGroup());
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// Strategy 2: Medical & Waiting Period Eligibility Rule
// This begins a reusable type or template: a blueprint for related data and behavior.
class EligibilityRule : public IMatchingRule {
// The following members are available to other parts of the program.
public:
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string ruleName() const override { return "Medical & Waiting Period Eligibility"; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool evaluate(const Donor& donor, const Request&) const override {
// This controls which part of the program runs next, based on the current condition or result.
        if (!donor.isProfileComplete()) return false;
// This controls which part of the program runs next, based on the current condition or result.
        if (donor.age() < 18 || donor.age() > 65) return false;
// This controls which part of the program runs next, based on the current condition or result.
        if (donor.weight() < 50.0) return false;
// This sends a result back to the code that called this function.
        return donor.isEligibleOnDate(); // Checks 90/120 day waiting period
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// Strategy 3: Privacy Consent Rule
// This begins a reusable type or template: a blueprint for related data and behavior.
class ConsentRule : public IMatchingRule {
// The following members are available to other parts of the program.
public:
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string ruleName() const override { return "Privacy Consent"; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool evaluate(const Donor& donor, const Request&) const override {
// This sends a result back to the code that called this function.
        return donor.hasConsent();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// Strategy 4: Optional Location Rule (Hospital proximity)
// This begins a reusable type or template: a blueprint for related data and behavior.
class LocationRule : public IMatchingRule {
// This creates or updates a value that the following logic uses.
    std::string targetCity_;
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit LocationRule(std::string targetCity = "")
// This is part of the current expression or block; read it together with the nearby lines.
        : targetCity_(std::move(targetCity)) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string ruleName() const override { return "Location Proximity"; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool evaluate(const Donor&, const Request&) const override {
// This is a source comment that explains nearby code.
        // If no location restriction specified, pass all
// This controls which part of the program runs next, based on the current condition or result.
        if (targetCity_.empty()) return true;
// This sends a result back to the code that called this function.
        return true;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// Composite Context: DonorMatchingEngine
// This begins a reusable type or template: a blueprint for related data and behavior.
class DonorMatchingEngine {
// This creates or updates a value that the following logic uses.
    std::vector<std::unique_ptr<IMatchingRule>> rules_;

// The following members are available to other parts of the program.
public:
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    DonorMatchingEngine() {
// This is a source comment that explains nearby code.
        // Default rule pipeline
// This performs one step of the current operation, then continues to the next statement.
        rules_.push_back(std::make_unique<BloodCompatibilityRule>());
// This performs one step of the current operation, then continues to the next statement.
        rules_.push_back(std::make_unique<EligibilityRule>());
// This performs one step of the current operation, then continues to the next statement.
        rules_.push_back(std::make_unique<ConsentRule>());
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void addRule(std::unique_ptr<IMatchingRule> rule) {
// This performs one step of the current operation, then continues to the next statement.
        rules_.push_back(std::move(rule));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This creates or updates a value that the following logic uses.
    std::vector<const Donor*> findMatches(const std::vector<Donor>& donors,
// This creates or updates a value that the following logic uses.
                                          const Request& request) const {
// This creates or updates a value that the following logic uses.
        std::vector<const Donor*> matches;
// This controls which part of the program runs next, based on the current condition or result.
        for (const auto& donor : donors) {
// This creates or updates a value that the following logic uses.
            bool passed = true;
// This controls which part of the program runs next, based on the current condition or result.
            for (const auto& rule : rules_) {
// This controls which part of the program runs next, based on the current condition or result.
                if (!rule->evaluate(donor, request)) {
// This performs one step of the current operation, then continues to the next statement.
                    passed = false;
// This changes the current loop flow: stop this loop, or move to its next iteration.
                    break;
// This closes the block opened above, such as a function, class, or control-flow branch.
                }
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This controls which part of the program runs next, based on the current condition or result.
            if (passed) {
// This performs one step of the current operation, then continues to the next statement.
                matches.push_back(&donor);
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return matches;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
