#pragma once
#include "bloodline/Donor.hpp"
#include "bloodline/Request.hpp"
#include "bloodline/Compatibility.hpp"
#include <vector>
#include <memory>
#include <string>

namespace bloodline {

// Design Pattern: Strategy Pattern for Donor Matching.
// IMatchingRule provides an extensible interface. New rules can be plugged in
// without modifying the core DonorMatchingEngine.

class IMatchingRule {
public:
    virtual ~IMatchingRule() = default;
    virtual std::string ruleName() const = 0;
    virtual bool evaluate(const Donor& donor, const Request& request) const = 0;
};

// Strategy 1: Blood Compatibility Rule using CompatibilityTable
class BloodCompatibilityRule : public IMatchingRule {
    CompatibilityTable table_;
public:
    std::string ruleName() const override { return "Blood Compatibility"; }
    bool evaluate(const Donor& donor, const Request& request) const override {
        if (!donor.bloodGroup()) return false;
        return table_.canDonateTo(*donor.bloodGroup(), request.bloodGroup());
    }
};

// Strategy 2: Medical & Waiting Period Eligibility Rule
class EligibilityRule : public IMatchingRule {
public:
    std::string ruleName() const override { return "Medical & Waiting Period Eligibility"; }
    bool evaluate(const Donor& donor, const Request&) const override {
        if (!donor.isProfileComplete()) return false;
        if (donor.age() < 18 || donor.age() > 65) return false;
        if (donor.weight() < 50.0) return false;
        return donor.isEligibleOnDate(); // Checks 90/120 day waiting period
    }
};

// Strategy 3: Privacy Consent Rule
class ConsentRule : public IMatchingRule {
public:
    std::string ruleName() const override { return "Privacy Consent"; }
    bool evaluate(const Donor& donor, const Request&) const override {
        return donor.hasConsent();
    }
};

// Strategy 4: Optional Location Rule (Hospital proximity)
class LocationRule : public IMatchingRule {
    std::string targetCity_;
public:
    explicit LocationRule(std::string targetCity = "")
        : targetCity_(std::move(targetCity)) {}

    std::string ruleName() const override { return "Location Proximity"; }
    bool evaluate(const Donor&, const Request&) const override {
        // If no location restriction specified, pass all
        if (targetCity_.empty()) return true;
        return true;
    }
};

// Composite Context: DonorMatchingEngine
class DonorMatchingEngine {
    std::vector<std::unique_ptr<IMatchingRule>> rules_;

public:
    DonorMatchingEngine() {
        // Default rule pipeline
        rules_.push_back(std::make_unique<BloodCompatibilityRule>());
        rules_.push_back(std::make_unique<EligibilityRule>());
        rules_.push_back(std::make_unique<ConsentRule>());
    }

    void addRule(std::unique_ptr<IMatchingRule> rule) {
        rules_.push_back(std::move(rule));
    }

    std::vector<const Donor*> findMatches(const std::vector<Donor>& donors,
                                          const Request& request) const {
        std::vector<const Donor*> matches;
        for (const auto& donor : donors) {
            bool passed = true;
            for (const auto& rule : rules_) {
                if (!rule->evaluate(donor, request)) {
                    passed = false;
                    break;
                }
            }
            if (passed) {
                matches.push_back(&donor);
            }
        }
        return matches;
    }
};

} // namespace bloodline
