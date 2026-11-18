// EXPLAINED REFERENCE COPY. The original source is in ../backend/tests/test_bloodline.cpp and remains unchanged.
// This brings in "bloodline/BloodGroup.hpp", so this file can use its declarations.
#include "bloodline/BloodGroup.hpp"
// This brings in "bloodline/Credential.hpp", so this file can use its declarations.
#include "bloodline/Credential.hpp"
// This brings in "bloodline/Donor.hpp", so this file can use its declarations.
#include "bloodline/Donor.hpp"
// This brings in "bloodline/Organization.hpp", so this file can use its declarations.
#include "bloodline/Organization.hpp"
// This brings in "bloodline/Camp.hpp", so this file can use its declarations.
#include "bloodline/Camp.hpp"
// This brings in "bloodline/Donation.hpp", so this file can use its declarations.
#include "bloodline/Donation.hpp"
// This brings in "bloodline/BloodUnit.hpp", so this file can use its declarations.
#include "bloodline/BloodUnit.hpp"
// This brings in "bloodline/Request.hpp", so this file can use its declarations.
#include "bloodline/Request.hpp"
// This brings in "bloodline/MatchingEngine.hpp", so this file can use its declarations.
#include "bloodline/MatchingEngine.hpp"
// This brings in "bloodline/AccountFactory.hpp", so this file can use its declarations.
#include "bloodline/AccountFactory.hpp"
// This brings in "bloodline/Compatibility.hpp", so this file can use its declarations.
#include "bloodline/Compatibility.hpp"
// This brings in "bloodline/EligibilityRules.hpp", so this file can use its declarations.
#include "bloodline/EligibilityRules.hpp"
// This brings in "bloodline/IRepository.hpp", so this file can use its declarations.
#include "bloodline/IRepository.hpp"
// This brings in "bloodline/FileRepository.hpp", so this file can use its declarations.
#include "bloodline/FileRepository.hpp"
// This brings in "bloodline/SessionManager.hpp", so this file can use its declarations.
#include "bloodline/SessionManager.hpp"
// This brings in "bloodline/Errors.hpp", so this file can use its declarations.
#include "bloodline/Errors.hpp"
// This brings in "bloodline/DateUtil.hpp", so this file can use its declarations.
#include "bloodline/DateUtil.hpp"

// This brings in <iostream>, so this file can use its declarations.
#include <iostream>
// This brings in <sstream>, so this file can use its declarations.
#include <sstream>
// This brings in <cassert>, so this file can use its declarations.
#include <cassert>
// This brings in <memory>, so this file can use its declarations.
#include <memory>
// This brings in <vector>, so this file can use its declarations.
#include <vector>
// This brings in <cmath>, so this file can use its declarations.
#include <cmath>

// This is a source comment that explains nearby code.
// Minimal test framework: no external dependencies
// This creates or updates a value that the following logic uses.
static int tests_passed = 0;
// This creates or updates a value that the following logic uses.
static int tests_failed = 0;

// This groups related C++ names together to avoid clashes with other code.
namespace bl = bloodline;

// This is a preprocessor instruction that configures what the compiler sees.
#define TEST(name) \
// This is part of the current expression or block; read it together with the nearby lines.
    static void test_##name(); \
// This begins a reusable type or template: a blueprint for related data and behavior.
    struct Register_##name { \
// This is part of the current expression or block; read it together with the nearby lines.
        Register_##name() { \
// This is part of the current expression or block; read it together with the nearby lines.
            std::cout << "  [RUN ] " << #name << "\n"; \
// This controls which part of the program runs next, based on the current condition or result.
            try { \
// This is part of the current expression or block; read it together with the nearby lines.
                test_##name(); \
// This is part of the current expression or block; read it together with the nearby lines.
                std::cout << "  [PASS] " << #name << "\n"; \
// This is part of the current expression or block; read it together with the nearby lines.
                ++tests_passed; \
// This closes the block opened above, such as a function, class, or control-flow branch.
            } catch (const std::exception& e) { \
// This is part of the current expression or block; read it together with the nearby lines.
                std::cout << "  [FAIL] " << #name << ": " << e.what() << "\n"; \
// This is part of the current expression or block; read it together with the nearby lines.
                ++tests_failed; \
// This closes the block opened above, such as a function, class, or control-flow branch.
            } \
// This closes the block opened above, such as a function, class, or control-flow branch.
        } \
// This closes the block opened above, such as a function, class, or control-flow branch.
    } reg_##name; \
// This is part of the current expression or block; read it together with the nearby lines.
    static void test_##name()

// This is a preprocessor instruction that configures what the compiler sees.
#define ASSERT_TRUE(cond) \
// This is part of the current expression or block; read it together with the nearby lines.
    do { if (!(cond)) throw std::runtime_error( \
// This creates or updates a value that the following logic uses.
        std::string("Assertion failed: ") + #cond + \
// This is part of the current expression or block; read it together with the nearby lines.
        " at line " + std::to_string(__LINE__)); } while(0)

// This is a preprocessor instruction that configures what the compiler sees.
#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

// This is a preprocessor instruction that configures what the compiler sees.
#define ASSERT_EQ(a, b) \
// This is part of the current expression or block; read it together with the nearby lines.
    do { if ((a) != (b)) { \
// This is part of the current expression or block; read it together with the nearby lines.
        std::ostringstream oss; \
// This is part of the current expression or block; read it together with the nearby lines.
        oss << "Expected " << (a) << " == " << (b) \
// This is part of the current expression or block; read it together with the nearby lines.
            << " at line " << __LINE__; \
// This reports an error so the caller can handle the failed operation.
        throw std::runtime_error(oss.str()); \
// This closes the block opened above, such as a function, class, or control-flow branch.
    } } while(0)

// This is a preprocessor instruction that configures what the compiler sees.
#define ASSERT_THROWS(expr, exc_type) \
// This creates or updates a value that the following logic uses.
    do { bool caught = false; \
// This controls which part of the program runs next, based on the current condition or result.
    try { expr; } catch (const exc_type&) { caught = true; } \
// This controls which part of the program runs next, based on the current condition or result.
    if (!caught) throw std::runtime_error( \
// This creates or updates a value that the following logic uses.
        std::string("Expected exception ") + #exc_type + \
// This is part of the current expression or block; read it together with the nearby lines.
        " at line " + std::to_string(__LINE__)); } while(0)

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// BloodGroup Tests
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(BloodGroup_ParseValid) {
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(bl::bloodGroupToString(bl::bloodGroupFromString("A+")), "A+");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(bl::bloodGroupToString(bl::bloodGroupFromString("O-")), "O-");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(bl::bloodGroupToString(bl::bloodGroupFromString("AB+")), "AB+");
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(BloodGroup_ParseInvalid) {
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_THROWS(bl::bloodGroupFromString("invalid"), bl::BadRequestError);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_THROWS(bl::bloodGroupFromString(""), bl::BadRequestError);
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(BloodGroup_StreamOperator) {
// This performs one step of the current operation, then continues to the next statement.
    std::ostringstream oss;
// This performs one step of the current operation, then continues to the next statement.
    oss << bl::BloodGroup::ONeg;
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(oss.str(), "O-");
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Credential Tests
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(Credential_VerifyCorrectPassword) {
// This creates or updates a value that the following logic uses.
    auto cred = bl::Credential::fromPassword("secret123");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_TRUE(cred.verify("secret123"));
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(Credential_RejectWrongPassword) {
// This creates or updates a value that the following logic uses.
    auto cred = bl::Credential::fromPassword("secret123");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_FALSE(cred.verify("wrongpass"));
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(Credential_DifferentSaltsProduceDifferentHashes) {
// This creates or updates a value that the following logic uses.
    auto cred1 = bl::Credential::fromPassword("same_password");
// This creates or updates a value that the following logic uses.
    auto cred2 = bl::Credential::fromPassword("same_password");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_TRUE(cred1.salt() != cred2.salt());
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(Credential_RoundTripFromStored) {
// This creates or updates a value that the following logic uses.
    auto cred1 = bl::Credential::fromPassword("mypassword");
// This creates or updates a value that the following logic uses.
    auto cred2 = bl::Credential::fromStored(cred1.salt(), cred1.hash());
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_TRUE(cred2.verify("mypassword"));
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Compatibility Tests
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(Compatibility_ONeg_UniversalDonor) {
// This performs one step of the current operation, then continues to the next statement.
    bl::CompatibilityTable table;
// This controls which part of the program runs next, based on the current condition or result.
    for (auto bg : {bl::BloodGroup::APos, bl::BloodGroup::ANeg,
// This is part of the current expression or block; read it together with the nearby lines.
                    bl::BloodGroup::BPos, bl::BloodGroup::BNeg,
// This is part of the current expression or block; read it together with the nearby lines.
                    bl::BloodGroup::ABPos, bl::BloodGroup::ABNeg,
// This opens a scope for the related statements that follow.
                    bl::BloodGroup::OPos, bl::BloodGroup::ONeg}) {
// This performs one step of the current operation, then continues to the next statement.
        ASSERT_TRUE(table.canDonateTo(bl::BloodGroup::ONeg, bg));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(Compatibility_ABPos_UniversalRecipient) {
// This performs one step of the current operation, then continues to the next statement.
    bl::CompatibilityTable table;
// This controls which part of the program runs next, based on the current condition or result.
    for (auto bg : {bl::BloodGroup::APos, bl::BloodGroup::ANeg,
// This is part of the current expression or block; read it together with the nearby lines.
                    bl::BloodGroup::BPos, bl::BloodGroup::BNeg,
// This is part of the current expression or block; read it together with the nearby lines.
                    bl::BloodGroup::ABPos, bl::BloodGroup::ABNeg,
// This opens a scope for the related statements that follow.
                    bl::BloodGroup::OPos, bl::BloodGroup::ONeg}) {
// This performs one step of the current operation, then continues to the next statement.
        ASSERT_TRUE(table.canReceiveFrom(bl::BloodGroup::ABPos, bg));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Waiting Period Tests (Safety Boundaries)
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(WaitingPeriod_Male_90DaysBoundary) {
// This performs one step of the current operation, then continues to the next statement.
    bl::Donor male("m_user", "Male Donor", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
    male.setGender("Male");
// This performs one step of the current operation, then continues to the next statement.
    male.setLastDonationDate("2026-01-01");

// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(male.waitingPeriodDays(), 90);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(male.nextEligibleDate(), "2026-04-01");

// This is a source comment that explains nearby code.
    // Day 89: ineligible
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_FALSE(male.isEligibleOnDate("2026-03-31"));
// This is a source comment that explains nearby code.
    // Day 90: eligible!
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_TRUE(male.isEligibleOnDate("2026-04-01"));
// This is a source comment that explains nearby code.
    // Day 91: eligible!
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_TRUE(male.isEligibleOnDate("2026-04-02"));
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(WaitingPeriod_Female_120DaysBoundary) {
// This performs one step of the current operation, then continues to the next statement.
    bl::Donor female("f_user", "Female Donor", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
    female.setGender("Female");
// This performs one step of the current operation, then continues to the next statement.
    female.setLastDonationDate("2026-01-01");

// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(female.waitingPeriodDays(), 120);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(female.nextEligibleDate(), "2026-05-01");

// This is a source comment that explains nearby code.
    // Day 119: ineligible
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_FALSE(female.isEligibleOnDate("2026-04-30"));
// This is a source comment that explains nearby code.
    // Day 120: eligible!
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_TRUE(female.isEligibleOnDate("2026-05-01"));
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(Donor_PhoneMasking_Security) {
// This performs one step of the current operation, then continues to the next statement.
    bl::Donor d("d_user", "Test Donor", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
    d.setPhone("9876543210");
// This creates or updates a value that the following logic uses.
    std::string masked = d.maskedPhone();
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(masked, "98******10");
// This creates or updates a value that the following logic uses.
    ASSERT_TRUE(masked.find("7654") == std::string::npos);
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(Donor_MilestoneBadges) {
// This performs one step of the current operation, then continues to the next statement.
    bl::Donor d("d_user", "Test Donor", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
    d.setDonationCount(1);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(d.milestoneBadges().size(), 1u);
// This performs one step of the current operation, then continues to the next statement.
    d.setDonationCount(5);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(d.milestoneBadges().size(), 2u);
// This performs one step of the current operation, then continues to the next statement.
    d.setDonationCount(10);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(d.milestoneBadges().size(), 3u);
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// BloodUnit Shelf Life (42-day expiry)
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(BloodStock_Expiry_41DaysCounts_43DaysDoesNot) {
// This is a source comment that explains nearby code.
    // Collected 2026-01-01 -> expiry 2026-02-12 (42 days)
// This performs one step of the current operation, then continues to the next statement.
    bl::BloodUnit unit("U1", bl::BloodGroup::OPos, "2026-01-01", 42);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(unit.expiryDate(), "2026-02-12");

// This is a source comment that explains nearby code.
    // At 41 days (2026-02-11): active
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_FALSE(unit.isExpired("2026-02-11"));
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_TRUE(unit.daysRemaining("2026-02-11") >= 1);

// This is a source comment that explains nearby code.
    // At 43 days (2026-02-13): expired
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_TRUE(unit.isExpired("2026-02-13"));
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_TRUE(unit.daysRemaining("2026-02-13") < 0);
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Camp Status State Machine & Capacity Limits
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(Camp_StatusTransitions_ValidAndInvalid) {
// This performs one step of the current operation, then continues to the next statement.
    bl::Camp camp("C1", "City Camp", "2026-10-20", 50);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(camp.statusString(), "Upcoming");

// This performs one step of the current operation, then continues to the next statement.
    camp.setStatus(bl::CampStatus::Open);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(camp.statusString(), "Open");

// This performs one step of the current operation, then continues to the next statement.
    camp.setStatus(bl::CampStatus::Full);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(camp.statusString(), "Full");

// This performs one step of the current operation, then continues to the next statement.
    camp.setStatus(bl::CampStatus::Completed);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(camp.statusString(), "Completed");

// This is a source comment that explains nearby code.
    // Completed camp cannot be cancelled or reopened
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_THROWS(camp.setStatus(bl::CampStatus::Cancelled), bl::InvalidStateTransitionError);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_THROWS(camp.setStatus(bl::CampStatus::Open), bl::InvalidStateTransitionError);

// This performs one step of the current operation, then continues to the next statement.
    bl::Camp camp2("C2", "Canceled Camp", "2026-10-22", 50);
// This performs one step of the current operation, then continues to the next statement.
    camp2.setStatus(bl::CampStatus::Cancelled);
// This is a source comment that explains nearby code.
    // Cancelled camp cannot be completed
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_THROWS(camp2.setStatus(bl::CampStatus::Completed), bl::InvalidStateTransitionError);
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(Camp_SlotCapAndDoubleRegistration) {
// This is part of the current expression or block; read it together with the nearby lines.
    bl::Camp camp("C1", "Mini Camp", "2026-10-20", 2, 2); // Cap = 2
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(camp.remainingSlots(), 2);

// This performs one step of the current operation, then continues to the next statement.
    camp.registerDonor("donor1");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(camp.remainingSlots(), 1);

// This is a source comment that explains nearby code.
    // Double registration forbidden
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_THROWS(camp.registerDonor("donor1"), bl::ConflictError);

// This performs one step of the current operation, then continues to the next statement.
    camp.registerDonor("donor2");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(camp.remainingSlots(), 0);

// This is a source comment that explains nearby code.
    // Slots full -> rejects donor 3
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_THROWS(camp.registerDonor("donor3"), bl::ConflictError);
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Strategy Pattern Matching Engine Tests
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(MatchingEngine_UrgentRequest_FilterIneligibleAndNonConsented) {
// This performs one step of the current operation, then continues to the next statement.
    bl::DonorMatchingEngine engine;

// This is a source comment that explains nearby code.
    // Eligible, consented O- donor
// This performs one step of the current operation, then continues to the next statement.
    bl::Donor d1("d1", "Donor One", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
    d1.setAge(25); d1.setWeight(65); d1.setGender("Male");
// This performs one step of the current operation, then continues to the next statement.
    d1.setBloodGroup(bl::BloodGroup::ONeg);
// This performs one step of the current operation, then continues to the next statement.
    d1.grantConsent(); d1.markProfileComplete();

// This is a source comment that explains nearby code.
    // Ineligible: consent revoked
// This performs one step of the current operation, then continues to the next statement.
    bl::Donor d2("d2", "Donor Two", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
    d2.setAge(25); d2.setWeight(65); d2.setGender("Male");
// This performs one step of the current operation, then continues to the next statement.
    d2.setBloodGroup(bl::BloodGroup::ONeg);
// This performs one step of the current operation, then continues to the next statement.
    d2.revokeConsent(); d2.markProfileComplete();

// This is a source comment that explains nearby code.
    // Ineligible: recently donated
// This performs one step of the current operation, then continues to the next statement.
    bl::Donor d3("d3", "Donor Three", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
    d3.setAge(25); d3.setWeight(65); d3.setGender("Male");
// This performs one step of the current operation, then continues to the next statement.
    d3.setBloodGroup(bl::BloodGroup::ONeg);
// This performs one step of the current operation, then continues to the next statement.
    d3.grantConsent(); d3.markProfileComplete();
// This is part of the current expression or block; read it together with the nearby lines.
    d3.setLastDonationDate(bl::DateUtil::today()); // Donated today

// This is a source comment that explains nearby code.
    // Ineligible: incompatible blood group (AB+ cannot donate to O-)
// This performs one step of the current operation, then continues to the next statement.
    bl::Donor d4("d4", "Donor Four", bl::Credential{});
// This performs one step of the current operation, then continues to the next statement.
    d4.setAge(25); d4.setWeight(65); d4.setGender("Male");
// This performs one step of the current operation, then continues to the next statement.
    d4.setBloodGroup(bl::BloodGroup::ABPos);
// This performs one step of the current operation, then continues to the next statement.
    d4.grantConsent(); d4.markProfileComplete();

// This creates or updates a value that the following logic uses.
    std::vector<bl::Donor> pool = {d1, d2, d3, d4};

// This is part of the current expression or block; read it together with the nearby lines.
    bl::UrgentRequest req("REQ-1", "hospital_1", "District Hospital",
// This performs one step of the current operation, then continues to the next statement.
                          bl::BloodGroup::ONeg, 2, true);

// This creates or updates a value that the following logic uses.
    auto matches = engine.findMatches(pool, req);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(matches.size(), 1u);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(matches[0]->username(), "d1");
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Polymorphic Request Prioritization Tests
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(RequestPrioritizer_UrgentOverRoutine) {
// This creates or updates a value that the following logic uses.
    std::vector<std::unique_ptr<bl::Request>> reqs;
// This is part of the current expression or block; read it together with the nearby lines.
    reqs.push_back(std::make_unique<bl::RoutineRequest>(
// This performs one step of the current operation, then continues to the next statement.
        "R1", "org1", "Hosp 1", bl::BloodGroup::APos, 2, "2026-10-01"));
// This is part of the current expression or block; read it together with the nearby lines.
    reqs.push_back(std::make_unique<bl::UrgentRequest>(
// This is part of the current expression or block; read it together with the nearby lines.
        "R2", "org1", "Hosp 1", bl::BloodGroup::ONeg, 3, true, "2026-10-02")); // Critical (100)
// This is part of the current expression or block; read it together with the nearby lines.
    reqs.push_back(std::make_unique<bl::UrgentRequest>(
// This is part of the current expression or block; read it together with the nearby lines.
        "R3", "org1", "Hosp 1", bl::BloodGroup::BPos, 1, false, "2026-10-03")); // Urgent (75)

// This performs one step of the current operation, then continues to the next statement.
    bl::RequestPrioritizer::sort(reqs);

// This is part of the current expression or block; read it together with the nearby lines.
    ASSERT_EQ(reqs[0]->id(), "R2"); // Critical first
// This is part of the current expression or block; read it together with the nearby lines.
    ASSERT_EQ(reqs[1]->id(), "R3"); // Urgent second
// This is part of the current expression or block; read it together with the nearby lines.
    ASSERT_EQ(reqs[2]->id(), "R1"); // Routine last
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Rate Limiter Security Tests
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(RateLimiter_LockAfter5Attempts) {
// This performs one step of the current operation, then continues to the next statement.
    bl::RateLimiter limiter(5, 300);
// This creates or updates a value that the following logic uses.
    std::string user = "attacker";

// This controls which part of the program runs next, based on the current condition or result.
    for (int i = 0; i < 4; ++i) {
// This performs one step of the current operation, then continues to the next statement.
        limiter.recordFailure(user);
// This performs one step of the current operation, then continues to the next statement.
        ASSERT_FALSE(limiter.isLocked(user));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is part of the current expression or block; read it together with the nearby lines.
    limiter.recordFailure(user); // 5th failure
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_TRUE(limiter.isLocked(user));
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_TRUE(limiter.remainingLockSeconds(user) > 0);

// This is part of the current expression or block; read it together with the nearby lines.
    limiter.recordSuccess(user); // Reset on successful auth
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_FALSE(limiter.isLocked(user));
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Account Factory Pattern Tests
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(AccountFactory_CreatesCorrectSubclasses) {
// This creates or updates a value that the following logic uses.
    auto donor = bl::AccountFactory::createAccount(
// This performs one step of the current operation, then continues to the next statement.
        "donor", "john", "John Doe", bl::Credential::fromPassword("123"));
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(donor->role(), "donor");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(donor->dashboardKind(), "donor");

// This creates or updates a value that the following logic uses.
    auto admin = bl::AccountFactory::createAccount(
// This performs one step of the current operation, then continues to the next statement.
        "admin", "sysadmin", "Super Admin", bl::Credential::fromPassword("123"));
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(admin->role(), "admin");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(admin->dashboardKind(), "admin");

// This creates or updates a value that the following logic uses.
    auto ngo = bl::AccountFactory::createAccount(
// This performs one step of the current operation, then continues to the next statement.
        "ngo", "redcross", "Red Cross", bl::Credential::fromPassword("123"), "hospital");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(ngo->role(), "ngo");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(ngo->displayName(), "Red Cross (Hospital)");
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(Camp_UnregisterDonor_ReleasesSlot) {
// This performs one step of the current operation, then continues to the next statement.
    bl::Camp camp("CAMP-TEST", "Drive", "2026-10-20", 2, 2);
// This performs one step of the current operation, then continues to the next statement.
    camp.registerDonor("donorA");
// This performs one step of the current operation, then continues to the next statement.
    camp.registerDonor("donorB");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(camp.remainingSlots(), 0);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(camp.statusString(), "Full");

// This is a source comment that explains nearby code.
    // Unregister donorA -> slot released and status restored
// This performs one step of the current operation, then continues to the next statement.
    camp.unregisterDonor("donorA");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(camp.remainingSlots(), 1);
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(camp.statusString(), "Upcoming");

// This is a source comment that explains nearby code.
    // Cannot unregister again
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_THROWS(camp.unregisterDonor("donorA"), bl::NotFoundError);
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
TEST(Donation_StatusLifecycle) {
// This performs one step of the current operation, then continues to the next statement.
    bl::Donation d("R-999", "donor1", "Donor One", "O-", "CAMP-1", "Camp 1", "2026-10-10", "Completed");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(d.status(), "Completed");
// This performs one step of the current operation, then continues to the next statement.
    d.setStatus("Discarded");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(d.status(), "Discarded");
// This performs one step of the current operation, then continues to the next statement.
    d.setStatus("Transfused");
// This performs one step of the current operation, then continues to the next statement.
    ASSERT_EQ(d.status(), "Transfused");
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────
// This is a source comment that explains nearby code.
// Main Test Runner
// This is a source comment that explains nearby code.
// ───────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
int main() {
// This is part of the current expression or block; read it together with the nearby lines.
    std::cout << "\n========================================\n"
// This is part of the current expression or block; read it together with the nearby lines.
              << "  BloodLine Extended Test Suite (Phase 2)\n"
// This performs one step of the current operation, then continues to the next statement.
              << "========================================\n\n";

// This is part of the current expression or block; read it together with the nearby lines.
    std::cout << "\n========================================\n"
// This is part of the current expression or block; read it together with the nearby lines.
              << "  Results: " << tests_passed << " passed, "
// This is part of the current expression or block; read it together with the nearby lines.
              << tests_failed << " failed\n"
// This performs one step of the current operation, then continues to the next statement.
              << "========================================\n";

// This sends a result back to the code that called this function.
    return tests_failed > 0 ? 1 : 0;
// This closes the block opened above, such as a function, class, or control-flow branch.
}
