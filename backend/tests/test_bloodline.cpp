#include "bloodline/BloodGroup.hpp"
#include "bloodline/Credential.hpp"
#include "bloodline/Donor.hpp"
#include "bloodline/Organization.hpp"
#include "bloodline/Camp.hpp"
#include "bloodline/Donation.hpp"
#include "bloodline/BloodUnit.hpp"
#include "bloodline/Request.hpp"
#include "bloodline/MatchingEngine.hpp"
#include "bloodline/AccountFactory.hpp"
#include "bloodline/Compatibility.hpp"
#include "bloodline/EligibilityRules.hpp"
#include "bloodline/IRepository.hpp"
#include "bloodline/FileRepository.hpp"
#include "bloodline/SessionManager.hpp"
#include "bloodline/Errors.hpp"
#include "bloodline/DateUtil.hpp"

#include <iostream>
#include <sstream>
#include <cassert>
#include <memory>
#include <vector>
#include <cmath>

// Minimal test framework: no external dependencies
static int tests_passed = 0;
static int tests_failed = 0;

namespace bl = bloodline;

#define TEST(name) \
    static void test_##name(); \
    struct Register_##name { \
        Register_##name() { \
            std::cout << "  [RUN ] " << #name << "\n"; \
            try { \
                test_##name(); \
                std::cout << "  [PASS] " << #name << "\n"; \
                ++tests_passed; \
            } catch (const std::exception& e) { \
                std::cout << "  [FAIL] " << #name << ": " << e.what() << "\n"; \
                ++tests_failed; \
            } \
        } \
    } reg_##name; \
    static void test_##name()

#define ASSERT_TRUE(cond) \
    do { if (!(cond)) throw std::runtime_error( \
        std::string("Assertion failed: ") + #cond + \
        " at line " + std::to_string(__LINE__)); } while(0)

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define ASSERT_EQ(a, b) \
    do { if ((a) != (b)) { \
        std::ostringstream oss; \
        oss << "Expected " << (a) << " == " << (b) \
            << " at line " << __LINE__; \
        throw std::runtime_error(oss.str()); \
    } } while(0)

#define ASSERT_THROWS(expr, exc_type) \
    do { bool caught = false; \
    try { expr; } catch (const exc_type&) { caught = true; } \
    if (!caught) throw std::runtime_error( \
        std::string("Expected exception ") + #exc_type + \
        " at line " + std::to_string(__LINE__)); } while(0)

// ───────────────────────────────────────────────
// BloodGroup Tests
// ───────────────────────────────────────────────

TEST(BloodGroup_ParseValid) {
    ASSERT_EQ(bl::bloodGroupToString(bl::bloodGroupFromString("A+")), "A+");
    ASSERT_EQ(bl::bloodGroupToString(bl::bloodGroupFromString("O-")), "O-");
    ASSERT_EQ(bl::bloodGroupToString(bl::bloodGroupFromString("AB+")), "AB+");
}

TEST(BloodGroup_ParseInvalid) {
    ASSERT_THROWS(bl::bloodGroupFromString("invalid"), bl::BadRequestError);
    ASSERT_THROWS(bl::bloodGroupFromString(""), bl::BadRequestError);
}

TEST(BloodGroup_StreamOperator) {
    std::ostringstream oss;
    oss << bl::BloodGroup::ONeg;
    ASSERT_EQ(oss.str(), "O-");
}

// ───────────────────────────────────────────────
// Credential Tests
// ───────────────────────────────────────────────

TEST(Credential_VerifyCorrectPassword) {
    auto cred = bl::Credential::fromPassword("secret123");
    ASSERT_TRUE(cred.verify("secret123"));
}

TEST(Credential_RejectWrongPassword) {
    auto cred = bl::Credential::fromPassword("secret123");
    ASSERT_FALSE(cred.verify("wrongpass"));
}

TEST(Credential_DifferentSaltsProduceDifferentHashes) {
    auto cred1 = bl::Credential::fromPassword("same_password");
    auto cred2 = bl::Credential::fromPassword("same_password");
    ASSERT_TRUE(cred1.salt() != cred2.salt());
}

TEST(Credential_RoundTripFromStored) {
    auto cred1 = bl::Credential::fromPassword("mypassword");
    auto cred2 = bl::Credential::fromStored(cred1.salt(), cred1.hash());
    ASSERT_TRUE(cred2.verify("mypassword"));
}

// ───────────────────────────────────────────────
// Compatibility Tests
// ───────────────────────────────────────────────

TEST(Compatibility_ONeg_UniversalDonor) {
    bl::CompatibilityTable table;
    for (auto bg : {bl::BloodGroup::APos, bl::BloodGroup::ANeg,
                    bl::BloodGroup::BPos, bl::BloodGroup::BNeg,
                    bl::BloodGroup::ABPos, bl::BloodGroup::ABNeg,
                    bl::BloodGroup::OPos, bl::BloodGroup::ONeg}) {
        ASSERT_TRUE(table.canDonateTo(bl::BloodGroup::ONeg, bg));
    }
}

TEST(Compatibility_ABPos_UniversalRecipient) {
    bl::CompatibilityTable table;
    for (auto bg : {bl::BloodGroup::APos, bl::BloodGroup::ANeg,
                    bl::BloodGroup::BPos, bl::BloodGroup::BNeg,
                    bl::BloodGroup::ABPos, bl::BloodGroup::ABNeg,
                    bl::BloodGroup::OPos, bl::BloodGroup::ONeg}) {
        ASSERT_TRUE(table.canReceiveFrom(bl::BloodGroup::ABPos, bg));
    }
}

// ───────────────────────────────────────────────
// Waiting Period Tests (Safety Boundaries)
// ───────────────────────────────────────────────

TEST(WaitingPeriod_Male_90DaysBoundary) {
    bl::Donor male("m_user", "Male Donor", bl::Credential{});
    male.setGender("Male");
    male.setLastDonationDate("2026-01-01");

    ASSERT_EQ(male.waitingPeriodDays(), 90);
    ASSERT_EQ(male.nextEligibleDate(), "2026-04-01");

    // Day 89: ineligible
    ASSERT_FALSE(male.isEligibleOnDate("2026-03-31"));
    // Day 90: eligible!
    ASSERT_TRUE(male.isEligibleOnDate("2026-04-01"));
    // Day 91: eligible!
    ASSERT_TRUE(male.isEligibleOnDate("2026-04-02"));
}

TEST(WaitingPeriod_Female_120DaysBoundary) {
    bl::Donor female("f_user", "Female Donor", bl::Credential{});
    female.setGender("Female");
    female.setLastDonationDate("2026-01-01");

    ASSERT_EQ(female.waitingPeriodDays(), 120);
    ASSERT_EQ(female.nextEligibleDate(), "2026-05-01");

    // Day 119: ineligible
    ASSERT_FALSE(female.isEligibleOnDate("2026-04-30"));
    // Day 120: eligible!
    ASSERT_TRUE(female.isEligibleOnDate("2026-05-01"));
}

TEST(Donor_PhoneMasking_Security) {
    bl::Donor d("d_user", "Test Donor", bl::Credential{});
    d.setPhone("9876543210");
    std::string masked = d.maskedPhone();
    ASSERT_EQ(masked, "98******10");
    ASSERT_TRUE(masked.find("7654") == std::string::npos);
}

TEST(Donor_MilestoneBadges) {
    bl::Donor d("d_user", "Test Donor", bl::Credential{});
    d.setDonationCount(1);
    ASSERT_EQ(d.milestoneBadges().size(), 1u);
    d.setDonationCount(5);
    ASSERT_EQ(d.milestoneBadges().size(), 2u);
    d.setDonationCount(10);
    ASSERT_EQ(d.milestoneBadges().size(), 3u);
}

// ───────────────────────────────────────────────
// BloodUnit Shelf Life (42-day expiry)
// ───────────────────────────────────────────────

TEST(BloodStock_Expiry_41DaysCounts_43DaysDoesNot) {
    // Collected 2026-01-01 -> expiry 2026-02-12 (42 days)
    bl::BloodUnit unit("U1", bl::BloodGroup::OPos, "2026-01-01", 42);
    ASSERT_EQ(unit.expiryDate(), "2026-02-12");

    // At 41 days (2026-02-11): active
    ASSERT_FALSE(unit.isExpired("2026-02-11"));
    ASSERT_TRUE(unit.daysRemaining("2026-02-11") >= 1);

    // At 43 days (2026-02-13): expired
    ASSERT_TRUE(unit.isExpired("2026-02-13"));
    ASSERT_TRUE(unit.daysRemaining("2026-02-13") < 0);
}

// ───────────────────────────────────────────────
// Camp Status State Machine & Capacity Limits
// ───────────────────────────────────────────────

TEST(Camp_StatusTransitions_ValidAndInvalid) {
    bl::Camp camp("C1", "City Camp", "2026-10-20", 50);
    ASSERT_EQ(camp.statusString(), "Upcoming");

    camp.setStatus(bl::CampStatus::Open);
    ASSERT_EQ(camp.statusString(), "Open");

    camp.setStatus(bl::CampStatus::Full);
    ASSERT_EQ(camp.statusString(), "Full");

    camp.setStatus(bl::CampStatus::Completed);
    ASSERT_EQ(camp.statusString(), "Completed");

    // Completed camp cannot be cancelled or reopened
    ASSERT_THROWS(camp.setStatus(bl::CampStatus::Cancelled), bl::InvalidStateTransitionError);
    ASSERT_THROWS(camp.setStatus(bl::CampStatus::Open), bl::InvalidStateTransitionError);

    bl::Camp camp2("C2", "Canceled Camp", "2026-10-22", 50);
    camp2.setStatus(bl::CampStatus::Cancelled);
    // Cancelled camp cannot be completed
    ASSERT_THROWS(camp2.setStatus(bl::CampStatus::Completed), bl::InvalidStateTransitionError);
}

TEST(Camp_SlotCapAndDoubleRegistration) {
    bl::Camp camp("C1", "Mini Camp", "2026-10-20", 2, 2); // Cap = 2
    ASSERT_EQ(camp.remainingSlots(), 2);

    camp.registerDonor("donor1");
    ASSERT_EQ(camp.remainingSlots(), 1);

    // Double registration forbidden
    ASSERT_THROWS(camp.registerDonor("donor1"), bl::ConflictError);

    camp.registerDonor("donor2");
    ASSERT_EQ(camp.remainingSlots(), 0);

    // Slots full -> rejects donor 3
    ASSERT_THROWS(camp.registerDonor("donor3"), bl::ConflictError);
}

// ───────────────────────────────────────────────
// Strategy Pattern Matching Engine Tests
// ───────────────────────────────────────────────

TEST(MatchingEngine_UrgentRequest_FilterIneligibleAndNonConsented) {
    bl::DonorMatchingEngine engine;

    // Eligible, consented O- donor
    bl::Donor d1("d1", "Donor One", bl::Credential{});
    d1.setAge(25); d1.setWeight(65); d1.setGender("Male");
    d1.setBloodGroup(bl::BloodGroup::ONeg);
    d1.grantConsent(); d1.markProfileComplete();

    // Ineligible: consent revoked
    bl::Donor d2("d2", "Donor Two", bl::Credential{});
    d2.setAge(25); d2.setWeight(65); d2.setGender("Male");
    d2.setBloodGroup(bl::BloodGroup::ONeg);
    d2.revokeConsent(); d2.markProfileComplete();

    // Ineligible: recently donated
    bl::Donor d3("d3", "Donor Three", bl::Credential{});
    d3.setAge(25); d3.setWeight(65); d3.setGender("Male");
    d3.setBloodGroup(bl::BloodGroup::ONeg);
    d3.grantConsent(); d3.markProfileComplete();
    d3.setLastDonationDate(bl::DateUtil::today()); // Donated today

    // Ineligible: incompatible blood group (AB+ cannot donate to O-)
    bl::Donor d4("d4", "Donor Four", bl::Credential{});
    d4.setAge(25); d4.setWeight(65); d4.setGender("Male");
    d4.setBloodGroup(bl::BloodGroup::ABPos);
    d4.grantConsent(); d4.markProfileComplete();

    std::vector<bl::Donor> pool = {d1, d2, d3, d4};

    bl::UrgentRequest req("REQ-1", "hospital_1", "District Hospital",
                          bl::BloodGroup::ONeg, 2, true);

    auto matches = engine.findMatches(pool, req);
    ASSERT_EQ(matches.size(), 1u);
    ASSERT_EQ(matches[0]->username(), "d1");
}

// ───────────────────────────────────────────────
// Polymorphic Request Prioritization Tests
// ───────────────────────────────────────────────

TEST(RequestPrioritizer_UrgentOverRoutine) {
    std::vector<std::unique_ptr<bl::Request>> reqs;
    reqs.push_back(std::make_unique<bl::RoutineRequest>(
        "R1", "org1", "Hosp 1", bl::BloodGroup::APos, 2, "2026-10-01"));
    reqs.push_back(std::make_unique<bl::UrgentRequest>(
        "R2", "org1", "Hosp 1", bl::BloodGroup::ONeg, 3, true, "2026-10-02")); // Critical (100)
    reqs.push_back(std::make_unique<bl::UrgentRequest>(
        "R3", "org1", "Hosp 1", bl::BloodGroup::BPos, 1, false, "2026-10-03")); // Urgent (75)

    bl::RequestPrioritizer::sort(reqs);

    ASSERT_EQ(reqs[0]->id(), "R2"); // Critical first
    ASSERT_EQ(reqs[1]->id(), "R3"); // Urgent second
    ASSERT_EQ(reqs[2]->id(), "R1"); // Routine last
}

// ───────────────────────────────────────────────
// Rate Limiter Security Tests
// ───────────────────────────────────────────────

TEST(RateLimiter_LockAfter5Attempts) {
    bl::RateLimiter limiter(5, 300);
    std::string user = "attacker";

    for (int i = 0; i < 4; ++i) {
        limiter.recordFailure(user);
        ASSERT_FALSE(limiter.isLocked(user));
    }

    limiter.recordFailure(user); // 5th failure
    ASSERT_TRUE(limiter.isLocked(user));
    ASSERT_TRUE(limiter.remainingLockSeconds(user) > 0);

    limiter.recordSuccess(user); // Reset on successful auth
    ASSERT_FALSE(limiter.isLocked(user));
}

// ───────────────────────────────────────────────
// Account Factory Pattern Tests
// ───────────────────────────────────────────────

TEST(AccountFactory_CreatesCorrectSubclasses) {
    auto donor = bl::AccountFactory::createAccount(
        "donor", "john", "John Doe", bl::Credential::fromPassword("123"));
    ASSERT_EQ(donor->role(), "donor");
    ASSERT_EQ(donor->dashboardKind(), "donor");

    auto admin = bl::AccountFactory::createAccount(
        "admin", "sysadmin", "Super Admin", bl::Credential::fromPassword("123"));
    ASSERT_EQ(admin->role(), "admin");
    ASSERT_EQ(admin->dashboardKind(), "admin");

    auto ngo = bl::AccountFactory::createAccount(
        "ngo", "redcross", "Red Cross", bl::Credential::fromPassword("123"), "hospital");
    ASSERT_EQ(ngo->role(), "ngo");
    ASSERT_EQ(ngo->displayName(), "Red Cross (Hospital)");
}

TEST(Camp_UnregisterDonor_ReleasesSlot) {
    bl::Camp camp("CAMP-TEST", "Drive", "2026-10-20", 2, 2);
    camp.registerDonor("donorA");
    camp.registerDonor("donorB");
    ASSERT_EQ(camp.remainingSlots(), 0);
    ASSERT_EQ(camp.statusString(), "Full");

    // Unregister donorA -> slot released and status restored
    camp.unregisterDonor("donorA");
    ASSERT_EQ(camp.remainingSlots(), 1);
    ASSERT_EQ(camp.statusString(), "Upcoming");

    // Cannot unregister again
    ASSERT_THROWS(camp.unregisterDonor("donorA"), bl::NotFoundError);
}

TEST(Donation_StatusLifecycle) {
    bl::Donation d("R-999", "donor1", "Donor One", "O-", "CAMP-1", "Camp 1", "2026-10-10", "Completed");
    ASSERT_EQ(d.status(), "Completed");
    d.setStatus("Discarded");
    ASSERT_EQ(d.status(), "Discarded");
    d.setStatus("Transfused");
    ASSERT_EQ(d.status(), "Transfused");
}

// ───────────────────────────────────────────────
// Main Test Runner
// ───────────────────────────────────────────────

int main() {
    std::cout << "\n========================================\n"
              << "  BloodLine Extended Test Suite (Phase 2)\n"
              << "========================================\n\n";

    std::cout << "\n========================================\n"
              << "  Results: " << tests_passed << " passed, "
              << tests_failed << " failed\n"
              << "========================================\n";

    return tests_failed > 0 ? 1 : 0;
}
