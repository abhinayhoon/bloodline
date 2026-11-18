// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/BloodGroup.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <stdexcept>, so this file can use its declarations.
#include <stdexcept>
// This brings in <iostream>, so this file can use its declarations.
#include <iostream>
// This brings in <array>, so this file can use its declarations.
#include <array>
// This brings in "bloodline/Errors.hpp", so this file can use its declarations.
#include "bloodline/Errors.hpp"

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// OOP: ENCAPSULATION — BloodGroup is an enum class, not a raw string.
// This is a source comment that explains nearby code.
// Invalid values cannot be constructed; conversion goes through validated functions.
// This begins a reusable type or template: a blueprint for related data and behavior.
enum class BloodGroup {
// This is part of the current expression or block; read it together with the nearby lines.
    APos, ANeg, BPos, BNeg, ABPos, ABNeg, OPos, ONeg
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This opens a scope for the related statements that follow.
inline constexpr std::array<BloodGroup, 8> kAllBloodGroups = {
// This is part of the current expression or block; read it together with the nearby lines.
    BloodGroup::APos, BloodGroup::ANeg,
// This is part of the current expression or block; read it together with the nearby lines.
    BloodGroup::BPos, BloodGroup::BNeg,
// This is part of the current expression or block; read it together with the nearby lines.
    BloodGroup::ABPos, BloodGroup::ABNeg,
// This is part of the current expression or block; read it together with the nearby lines.
    BloodGroup::OPos, BloodGroup::ONeg
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
inline std::string bloodGroupToString(BloodGroup bg) {
// This controls which part of the program runs next, based on the current condition or result.
    switch (bg) {
// This controls which part of the program runs next, based on the current condition or result.
        case BloodGroup::APos:  return "A+";
// This controls which part of the program runs next, based on the current condition or result.
        case BloodGroup::ANeg:  return "A-";
// This controls which part of the program runs next, based on the current condition or result.
        case BloodGroup::BPos:  return "B+";
// This controls which part of the program runs next, based on the current condition or result.
        case BloodGroup::BNeg:  return "B-";
// This controls which part of the program runs next, based on the current condition or result.
        case BloodGroup::ABPos: return "AB+";
// This controls which part of the program runs next, based on the current condition or result.
        case BloodGroup::ABNeg: return "AB-";
// This controls which part of the program runs next, based on the current condition or result.
        case BloodGroup::OPos:  return "O+";
// This controls which part of the program runs next, based on the current condition or result.
        case BloodGroup::ONeg:  return "O-";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This sends a result back to the code that called this function.
    return "Unknown";
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
inline BloodGroup bloodGroupFromString(const std::string& str) {
// This controls which part of the program runs next, based on the current condition or result.
    if (str == "A+")  return BloodGroup::APos;
// This controls which part of the program runs next, based on the current condition or result.
    if (str == "A-")  return BloodGroup::ANeg;
// This controls which part of the program runs next, based on the current condition or result.
    if (str == "B+")  return BloodGroup::BPos;
// This controls which part of the program runs next, based on the current condition or result.
    if (str == "B-")  return BloodGroup::BNeg;
// This controls which part of the program runs next, based on the current condition or result.
    if (str == "AB+") return BloodGroup::ABPos;
// This controls which part of the program runs next, based on the current condition or result.
    if (str == "AB-") return BloodGroup::ABNeg;
// This controls which part of the program runs next, based on the current condition or result.
    if (str == "O+")  return BloodGroup::OPos;
// This controls which part of the program runs next, based on the current condition or result.
    if (str == "O-")  return BloodGroup::ONeg;
// This reports an error so the caller can handle the failed operation.
    throw BadRequestError("Invalid blood group: " + str);
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// OOP: POLYMORPHISM (operator overloading) — stream insertion for BloodGroup.
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
inline std::ostream& operator<<(std::ostream& os, BloodGroup bg) {
// This sends a result back to the code that called this function.
    return os << bloodGroupToString(bg);
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
