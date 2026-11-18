// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/BloodUnit.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in "bloodline/BloodGroup.hpp", so this file can use its declarations.
#include "bloodline/BloodGroup.hpp"
// This brings in "bloodline/DateUtil.hpp", so this file can use its declarations.
#include "bloodline/DateUtil.hpp"
// This brings in <nlohmann/json.hpp>, so this file can use its declarations.
#include <nlohmann/json.hpp>
// This brings in <string>, so this file can use its declarations.
#include <string>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// Encapsulation: stock expiry calculation lives inside BloodUnit, not in route handlers.
// This begins a reusable type or template: a blueprint for related data and behavior.
class BloodUnit {
// This creates or updates a value that the following logic uses.
    std::string id_;
// This performs one step of the current operation, then continues to the next statement.
    BloodGroup bloodGroup_;
// This creates or updates a value that the following logic uses.
    std::string collectionDate_;
// This creates or updates a value that the following logic uses.
    int shelfLifeDays_;
// This creates or updates a value that the following logic uses.
    std::string expiryDate_;

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    static constexpr int DEFAULT_SHELF_LIFE_DAYS = 42;

// This creates or updates a value that the following logic uses.
    BloodUnit(const std::string& id, BloodGroup bg, const std::string& collectionDate,
// This creates or updates a value that the following logic uses.
              int shelfLifeDays = DEFAULT_SHELF_LIFE_DAYS)
// This is part of the current expression or block; read it together with the nearby lines.
        : id_(id), bloodGroup_(bg), collectionDate_(collectionDate),
// This is part of the current expression or block; read it together with the nearby lines.
          shelfLifeDays_(shelfLifeDays),
// This is part of the current expression or block; read it together with the nearby lines.
          expiryDate_(DateUtil::addDays(collectionDate, shelfLifeDays)) {}

// This creates or updates a value that the following logic uses.
    const std::string& id() const noexcept { return id_; }
// This creates or updates a value that the following logic uses.
    BloodGroup bloodGroup() const noexcept { return bloodGroup_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string bloodGroupString() const { return bloodGroupToString(bloodGroup_); }
// This creates or updates a value that the following logic uses.
    const std::string& collectionDate() const noexcept { return collectionDate_; }
// This creates or updates a value that the following logic uses.
    const std::string& expiryDate() const noexcept { return expiryDate_; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int shelfLifeDays() const noexcept { return shelfLifeDays_; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool isExpired(const std::string& asOfDate = "") const {
// This creates or updates a value that the following logic uses.
        std::string refDate = asOfDate.empty() ? DateUtil::today() : asOfDate;
// This sends a result back to the code that called this function.
        return refDate > expiryDate_;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int daysRemaining(const std::string& asOfDate = "") const {
// This creates or updates a value that the following logic uses.
        std::string refDate = asOfDate.empty() ? DateUtil::today() : asOfDate;
// This sends a result back to the code that called this function.
        return DateUtil::daysBetween(refDate, expiryDate_);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    nlohmann::json toJson(const std::string& asOfDate = "") const {
// This creates or updates a value that the following logic uses.
        std::string refDate = asOfDate.empty() ? DateUtil::today() : asOfDate;
// This sends a result back to the code that called this function.
        return {
// This is part of the current expression or block; read it together with the nearby lines.
            {"id", id_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"bloodGroup", bloodGroupString()},
// This is part of the current expression or block; read it together with the nearby lines.
            {"collectionDate", collectionDate_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"expiryDate", expiryDate_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"shelfLifeDays", shelfLifeDays_},
// This is part of the current expression or block; read it together with the nearby lines.
            {"expired", isExpired(refDate)},
// This is part of the current expression or block; read it together with the nearby lines.
            {"daysRemaining", daysRemaining(refDate)}
// This closes the block opened above, such as a function, class, or control-flow branch.
        };
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
