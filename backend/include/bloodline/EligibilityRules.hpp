#pragma once
#include "bloodline/IEligibilityRule.hpp"
#include <string>
#include <sstream>
#include <optional>
#include <vector>
#include <memory>

namespace bloodline {

// OOP: INHERITANCE — ThresholdRule provides shared min/max/label logic.
// AgeRule and WeightRule derive from it, inheriting the threshold check.
// HemoglobinRule also derives from ThresholdRule but overrides check() for
// gender-dependent thresholds and the "pending" case.

// All thresholds are configured here. They should be verified against current
// national blood donation guidelines before use in any real system.

class ThresholdRule : public IEligibilityRule {
    std::string label_;
    double min_;
    double max_;
    std::string unit_;

protected:
    const std::string& label() const { return label_; }
    double min() const { return min_; }
    double max() const { return max_; }
    const std::string& unit() const { return unit_; }

    EligibilityResult makePass(double value) const {
        std::ostringstream oss;
        oss << label_ << ": " << value << unit_ << " (within " << min_ << "-" << max_ << ")";
        return {label_, "pass", oss.str()};
    }

    EligibilityResult makeFail(double value) const {
        std::ostringstream oss;
        oss << label_ << ": " << value << unit_ << " (must be " << min_ << "-" << max_ << ")";
        return {label_, "fail", oss.str()};
    }

public:
    ThresholdRule(std::string label, double min, double max, std::string unit)
        : label_(std::move(label)), min_(min), max_(max), unit_(std::move(unit)) {}
};

class AgeRule final : public ThresholdRule {
public:
    AgeRule() : ThresholdRule("Age", 18, 65, " years") {}

    EligibilityResult check(int age, const std::string&,
                            double, std::optional<double>) const override {
        double a = static_cast<double>(age);
        return (a >= min() && a <= max()) ? makePass(a) : makeFail(a);
    }
};

class WeightRule final : public ThresholdRule {
public:
    WeightRule() : ThresholdRule("Weight", 50, 300, " kg") {}

    EligibilityResult check(int, const std::string&,
                            double weight, std::optional<double>) const override {
        return (weight >= min() && weight <= max()) ? makePass(weight) : makeFail(weight);
    }
};

// Gender-dependent hemoglobin thresholds: 12.5 g/dL female, 13.0 g/dL male
class HemoglobinRule final : public ThresholdRule {
public:
    HemoglobinRule() : ThresholdRule("Hemoglobin", 12.5, 20.0, " g/dL") {}

    EligibilityResult check(int, const std::string& gender,
                            double, std::optional<double> hb) const override {
        if (!hb.has_value()) {
            return {label(), "pending", "Hemoglobin: to be checked at camp"};
        }
        double threshold = (gender == "Female") ? 12.5 : 13.0;
        if (*hb >= threshold) {
            std::ostringstream oss;
            oss << "Hemoglobin: " << *hb << " g/dL (threshold: " << threshold << ")";
            return {label(), "pass", oss.str()};
        }
        std::ostringstream oss;
        oss << "Hemoglobin: " << *hb << " g/dL (must be >= " << threshold << ")";
        return {label(), "fail", oss.str()};
    }
};

// OOP: POLYMORPHISM (runtime) — the engine runs a list of rules by virtual dispatch.
// It doesn't know or care what concrete rule types it holds.

class EligibilityEngine {
    std::vector<std::unique_ptr<IEligibilityRule>> rules_;

public:
    EligibilityEngine() {
        rules_.push_back(std::make_unique<AgeRule>());
        rules_.push_back(std::make_unique<WeightRule>());
        rules_.push_back(std::make_unique<HemoglobinRule>());
    }

    struct Result {
        bool eligible;
        std::vector<EligibilityResult> checks;
    };

    Result evaluate(int age, const std::string& gender,
                    double weight, std::optional<double> hemoglobin) const {
        Result result{true, {}};
        for (const auto& rule : rules_) {
            auto check = rule->check(age, gender, weight, hemoglobin);
            if (check.status == "fail") {
                result.eligible = false;
            }
            result.checks.push_back(std::move(check));
        }
        return result;
    }
};

} // namespace bloodline
