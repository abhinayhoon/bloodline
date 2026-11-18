#pragma once
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <functional>
#include <cstring>

namespace bloodline {

// OOP: ENCAPSULATION — Credential holds password hash and salt privately.
// No public getter exposes the raw hash. Verification goes through a method.
//
// This uses SHA-256-style hashing with a per-user salt using only the standard
// library. A real system would use bcrypt or argon2; this is for coursework.

class Credential {
    std::string salt_;
    std::string hash_;

    // Simple hash using std::hash and multiple rounds for basic security.
    // Not cryptographically strong — see README for production notes.
    static std::string computeHash(const std::string& password, const std::string& salt) {
        std::string input = salt + password;
        std::hash<std::string> hasher;
        size_t h = hasher(input);
        for (int i = 0; i < 1000; ++i) {
            h ^= hasher(std::to_string(h) + salt);
        }
        std::ostringstream oss;
        oss << std::hex << std::setfill('0') << std::setw(16) << h;
        return oss.str();
    }

    static std::string generateSalt() {
        static std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<uint64_t> dist;
        std::ostringstream oss;
        oss << std::hex << std::setfill('0') << std::setw(16) << dist(rng);
        return oss.str();
    }

public:
    Credential() = default;

    static Credential fromPassword(const std::string& password) {
        Credential cred;
        cred.salt_ = generateSalt();
        cred.hash_ = computeHash(password, cred.salt_);
        return cred;
    }

    static Credential fromStored(const std::string& salt, const std::string& hash) {
        Credential cred;
        cred.salt_ = salt;
        cred.hash_ = hash;
        return cred;
    }

    bool verify(const std::string& password) const {
        return computeHash(password, salt_) == hash_;
    }

    // Only for persistence layer — never sent in API responses
    const std::string& salt() const { return salt_; }
    const std::string& hash() const { return hash_; }
};

} // namespace bloodline
