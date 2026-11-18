#pragma once
#include <string>
#include <stdexcept>
#include <iostream>
#include <array>
#include "bloodline/Errors.hpp"

namespace bloodline {

// OOP: ENCAPSULATION — BloodGroup is an enum class, not a raw string.
// Invalid values cannot be constructed; conversion goes through validated functions.
enum class BloodGroup {
    APos, ANeg, BPos, BNeg, ABPos, ABNeg, OPos, ONeg
};

inline constexpr std::array<BloodGroup, 8> kAllBloodGroups = {
    BloodGroup::APos, BloodGroup::ANeg,
    BloodGroup::BPos, BloodGroup::BNeg,
    BloodGroup::ABPos, BloodGroup::ABNeg,
    BloodGroup::OPos, BloodGroup::ONeg
};

inline std::string bloodGroupToString(BloodGroup bg) {
    switch (bg) {
        case BloodGroup::APos:  return "A+";
        case BloodGroup::ANeg:  return "A-";
        case BloodGroup::BPos:  return "B+";
        case BloodGroup::BNeg:  return "B-";
        case BloodGroup::ABPos: return "AB+";
        case BloodGroup::ABNeg: return "AB-";
        case BloodGroup::OPos:  return "O+";
        case BloodGroup::ONeg:  return "O-";
    }
    return "Unknown";
}

inline BloodGroup bloodGroupFromString(const std::string& str) {
    if (str == "A+")  return BloodGroup::APos;
    if (str == "A-")  return BloodGroup::ANeg;
    if (str == "B+")  return BloodGroup::BPos;
    if (str == "B-")  return BloodGroup::BNeg;
    if (str == "AB+") return BloodGroup::ABPos;
    if (str == "AB-") return BloodGroup::ABNeg;
    if (str == "O+")  return BloodGroup::OPos;
    if (str == "O-")  return BloodGroup::ONeg;
    throw BadRequestError("Invalid blood group: " + str);
}

// OOP: POLYMORPHISM (operator overloading) — stream insertion for BloodGroup.
inline std::ostream& operator<<(std::ostream& os, BloodGroup bg) {
    return os << bloodGroupToString(bg);
}

} // namespace bloodline
