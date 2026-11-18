// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/Credential.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <random>, so this file can use its declarations.
#include <random>
// This brings in <sstream>, so this file can use its declarations.
#include <sstream>
// This brings in <iomanip>, so this file can use its declarations.
#include <iomanip>
// This brings in <functional>, so this file can use its declarations.
#include <functional>
// This brings in <cstring>, so this file can use its declarations.
#include <cstring>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// OOP: ENCAPSULATION — Credential holds password hash and salt privately.
// This is a source comment that explains nearby code.
// No public getter exposes the raw hash. Verification goes through a method.
// This is a source comment that explains nearby code.
//
// This is a source comment that explains nearby code.
// This uses SHA-256-style hashing with a per-user salt using only the standard
// This is a source comment that explains nearby code.
// library. A real system would use bcrypt or argon2; this is for coursework.

// This begins a reusable type or template: a blueprint for related data and behavior.
class Credential {
// This creates or updates a value that the following logic uses.
    std::string salt_;
// This creates or updates a value that the following logic uses.
    std::string hash_;

// This is a source comment that explains nearby code.
    // Simple hash using std::hash and multiple rounds for basic security.
// This is a source comment that explains nearby code.
    // Not cryptographically strong — see README for production notes.
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    static std::string computeHash(const std::string& password, const std::string& salt) {
// This creates or updates a value that the following logic uses.
        std::string input = salt + password;
// This creates or updates a value that the following logic uses.
        std::hash<std::string> hasher;
// This performs one step of the current operation, then continues to the next statement.
        size_t h = hasher(input);
// This controls which part of the program runs next, based on the current condition or result.
        for (int i = 0; i < 1000; ++i) {
// This performs one step of the current operation, then continues to the next statement.
            h ^= hasher(std::to_string(h) + salt);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        std::ostringstream oss;
// This performs one step of the current operation, then continues to the next statement.
        oss << std::hex << std::setfill('0') << std::setw(16) << h;
// This sends a result back to the code that called this function.
        return oss.str();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    static std::string generateSalt() {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        static std::mt19937_64 rng(std::random_device{}());
// This performs one step of the current operation, then continues to the next statement.
        std::uniform_int_distribution<uint64_t> dist;
// This performs one step of the current operation, then continues to the next statement.
        std::ostringstream oss;
// This performs one step of the current operation, then continues to the next statement.
        oss << std::hex << std::setfill('0') << std::setw(16) << dist(rng);
// This sends a result back to the code that called this function.
        return oss.str();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// The following members are available to other parts of the program.
public:
// This performs one step of the current operation, then continues to the next statement.
    Credential() = default;

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    static Credential fromPassword(const std::string& password) {
// This performs one step of the current operation, then continues to the next statement.
        Credential cred;
// This performs one step of the current operation, then continues to the next statement.
        cred.salt_ = generateSalt();
// This performs one step of the current operation, then continues to the next statement.
        cred.hash_ = computeHash(password, cred.salt_);
// This sends a result back to the code that called this function.
        return cred;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    static Credential fromStored(const std::string& salt, const std::string& hash) {
// This performs one step of the current operation, then continues to the next statement.
        Credential cred;
// This performs one step of the current operation, then continues to the next statement.
        cred.salt_ = salt;
// This performs one step of the current operation, then continues to the next statement.
        cred.hash_ = hash;
// This sends a result back to the code that called this function.
        return cred;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool verify(const std::string& password) const {
// This sends a result back to the code that called this function.
        return computeHash(password, salt_) == hash_;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Only for persistence layer — never sent in API responses
// This creates or updates a value that the following logic uses.
    const std::string& salt() const { return salt_; }
// This creates or updates a value that the following logic uses.
    const std::string& hash() const { return hash_; }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
