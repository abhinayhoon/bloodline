// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/EligibilityRules.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in "bloodline/IEligibilityRule.hpp", so this file can use its declarations.
#include "bloodline/IEligibilityRule.hpp"
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <sstream>, so this file can use its declarations.
#include <sstream>
// This brings in <optional>, so this file can use its declarations.
#include <optional>
// This brings in <vector>, so this file can use its declarations.
#include <vector>
// This brings in <memory>, so this file can use its declarations.
#include <memory>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// OOP: INHERITANCE — ThresholdRule provides shared min/max/label logic.
// This is a source comment that explains nearby code.
// AgeRule and WeightRule derive from it, inheriting the threshold check.
// This is a source comment that explains nearby code.
// HemoglobinRule also derives from ThresholdRule but overrides check() for
// This is a source comment that explains nearby code.
// gender-dependent thresholds and the "pending" case.

// This is a source comment that explains nearby code.
// All thresholds are configured here. They should be verified against current
// This is a source comment that explains nearby code.
// national blood donation guidelines before use in any real system.

// This begins a reusable type or template: a blueprint for related data and behavior.
class ThresholdRule : public IEligibilityRule {
// This creates or updates a value that the following logic uses.
    std::string label_;
// This creates or updates a value that the following logic uses.
    double min_;
// This creates or updates a value that the following logic uses.
    double max_;
// This creates or updates a value that the following logic uses.
    std::string unit_;

// The following members are shared with this class and its child classes.
protected:
// This creates or updates a value that the following logic uses.
    const std::string& label() const { return label_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    double min() const { return min_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    double max() const { return max_; }
// This creates or updates a value that the following logic uses.
    const std::string& unit() const { return unit_; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    EligibilityResult makePass(double value) const {
// This performs one step of the current operation, then continues to the next statement.
        std::ostringstream oss;
// This performs one step of the current operation, then continues to the next statement.
        oss << label_ << ": " << value << unit_ << " (within " << min_ << "-" << max_ << ")";
// This sends a result back to the code that called this function.
        return {label_, "pass", oss.str()};
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    EligibilityResult makeFail(double value) const {
// This performs one step of the current operation, then continues to the next statement.
        std::ostringstream oss;
// This performs one step of the current operation, then continues to the next statement.
        oss << label_ << ": " << value << unit_ << " (must be " << min_ << "-" << max_ << ")";
// This sends a result back to the code that called this function.
        return {label_, "fail", oss.str()};
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    ThresholdRule(std::string label, double min, double max, std::string unit)
// This is part of the current expression or block; read it together with the nearby lines.
        : label_(std::move(label)), min_(min), max_(max), unit_(std::move(unit)) {}
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This begins a reusable type or template: a blueprint for related data and behavior.
class AgeRule final : public ThresholdRule {
// The following members are available to other parts of the program.
public:
// This is part of the current expression or block; read it together with the nearby lines.
    AgeRule() : ThresholdRule("Age", 18, 65, " years") {}

// This creates or updates a value that the following logic uses.
    EligibilityResult check(int age, const std::string&,
// This creates or updates a value that the following logic uses.
                            double, std::optional<double>) const override {
// This creates or updates a value that the following logic uses.
        double a = static_cast<double>(age);
// This sends a result back to the code that called this function.
        return (a >= min() && a <= max()) ? makePass(a) : makeFail(a);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This begins a reusable type or template: a blueprint for related data and behavior.
class WeightRule final : public ThresholdRule {
// The following members are available to other parts of the program.
public:
// This is part of the current expression or block; read it together with the nearby lines.
    WeightRule() : ThresholdRule("Weight", 50, 300, " kg") {}

// This creates or updates a value that the following logic uses.
    EligibilityResult check(int, const std::string&,
// This creates or updates a value that the following logic uses.
                            double weight, std::optional<double>) const override {
// This sends a result back to the code that called this function.
        return (weight >= min() && weight <= max()) ? makePass(weight) : makeFail(weight);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// Gender-dependent hemoglobin thresholds: 12.5 g/dL female, 13.0 g/dL male
// This begins a reusable type or template: a blueprint for related data and behavior.
class HemoglobinRule final : public ThresholdRule {
// The following members are available to other parts of the program.
public:
// This is part of the current expression or block; read it together with the nearby lines.
    HemoglobinRule() : ThresholdRule("Hemoglobin", 12.5, 20.0, " g/dL") {}

// This creates or updates a value that the following logic uses.
    EligibilityResult check(int, const std::string& gender,
// This creates or updates a value that the following logic uses.
                            double, std::optional<double> hb) const override {
// This controls which part of the program runs next, based on the current condition or result.
        if (!hb.has_value()) {
// This sends a result back to the code that called this function.
            return {label(), "pending", "Hemoglobin: to be checked at camp"};
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This creates or updates a value that the following logic uses.
        double threshold = (gender == "Female") ? 12.5 : 13.0;
// This controls which part of the program runs next, based on the current condition or result.
        if (*hb >= threshold) {
// This performs one step of the current operation, then continues to the next statement.
            std::ostringstream oss;
// This performs one step of the current operation, then continues to the next statement.
            oss << "Hemoglobin: " << *hb << " g/dL (threshold: " << threshold << ")";
// This sends a result back to the code that called this function.
            return {label(), "pass", oss.str()};
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        std::ostringstream oss;
// This performs one step of the current operation, then continues to the next statement.
        oss << "Hemoglobin: " << *hb << " g/dL (must be >= " << threshold << ")";
// This sends a result back to the code that called this function.
        return {label(), "fail", oss.str()};
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// OOP: POLYMORPHISM (runtime) — the engine runs a list of rules by virtual dispatch.
// This is a source comment that explains nearby code.
// It doesn't know or care what concrete rule types it holds.

// This begins a reusable type or template: a blueprint for related data and behavior.
class EligibilityEngine {
// This creates or updates a value that the following logic uses.
    std::vector<std::unique_ptr<IEligibilityRule>> rules_;

// The following members are available to other parts of the program.
public:
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    EligibilityEngine() {
// This performs one step of the current operation, then continues to the next statement.
        rules_.push_back(std::make_unique<AgeRule>());
// This performs one step of the current operation, then continues to the next statement.
        rules_.push_back(std::make_unique<WeightRule>());
// This performs one step of the current operation, then continues to the next statement.
        rules_.push_back(std::make_unique<HemoglobinRule>());
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This begins a reusable type or template: a blueprint for related data and behavior.
    struct Result {
// This creates or updates a value that the following logic uses.
        bool eligible;
// This creates or updates a value that the following logic uses.
        std::vector<EligibilityResult> checks;
// This closes the block opened above, such as a function, class, or control-flow branch.
    };

// This creates or updates a value that the following logic uses.
    Result evaluate(int age, const std::string& gender,
// This creates or updates a value that the following logic uses.
                    double weight, std::optional<double> hemoglobin) const {
// This performs one step of the current operation, then continues to the next statement.
        Result result{true, {}};
// This controls which part of the program runs next, based on the current condition or result.
        for (const auto& rule : rules_) {
// This creates or updates a value that the following logic uses.
            auto check = rule->check(age, gender, weight, hemoglobin);
// This controls which part of the program runs next, based on the current condition or result.
            if (check.status == "fail") {
// This performs one step of the current operation, then continues to the next statement.
                result.eligible = false;
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This performs one step of the current operation, then continues to the next statement.
            result.checks.push_back(std::move(check));
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
