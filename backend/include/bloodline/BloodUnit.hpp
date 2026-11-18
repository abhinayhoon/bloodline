#pragma once
#include "bloodline/BloodGroup.hpp"
#include "bloodline/DateUtil.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace bloodline {

// Encapsulation: stock expiry calculation lives inside BloodUnit, not in route handlers.
class BloodUnit {
    std::string id_;
    BloodGroup bloodGroup_;
    std::string collectionDate_;
    int shelfLifeDays_;
    std::string expiryDate_;

public:
    static constexpr int DEFAULT_SHELF_LIFE_DAYS = 42;

    BloodUnit(const std::string& id, BloodGroup bg, const std::string& collectionDate,
              int shelfLifeDays = DEFAULT_SHELF_LIFE_DAYS)
        : id_(id), bloodGroup_(bg), collectionDate_(collectionDate),
          shelfLifeDays_(shelfLifeDays),
          expiryDate_(DateUtil::addDays(collectionDate, shelfLifeDays)) {}

    const std::string& id() const noexcept { return id_; }
    BloodGroup bloodGroup() const noexcept { return bloodGroup_; }
    std::string bloodGroupString() const { return bloodGroupToString(bloodGroup_); }
    const std::string& collectionDate() const noexcept { return collectionDate_; }
    const std::string& expiryDate() const noexcept { return expiryDate_; }
    int shelfLifeDays() const noexcept { return shelfLifeDays_; }

    bool isExpired(const std::string& asOfDate = "") const {
        std::string refDate = asOfDate.empty() ? DateUtil::today() : asOfDate;
        return refDate > expiryDate_;
    }

    int daysRemaining(const std::string& asOfDate = "") const {
        std::string refDate = asOfDate.empty() ? DateUtil::today() : asOfDate;
        return DateUtil::daysBetween(refDate, expiryDate_);
    }

    nlohmann::json toJson(const std::string& asOfDate = "") const {
        std::string refDate = asOfDate.empty() ? DateUtil::today() : asOfDate;
        return {
            {"id", id_},
            {"bloodGroup", bloodGroupString()},
            {"collectionDate", collectionDate_},
            {"expiryDate", expiryDate_},
            {"shelfLifeDays", shelfLifeDays_},
            {"expired", isExpired(refDate)},
            {"daysRemaining", daysRemaining(refDate)}
        };
    }
};

} // namespace bloodline
