// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/SessionManager.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <unordered_map>, so this file can use its declarations.
#include <unordered_map>
// This brings in <random>, so this file can use its declarations.
#include <random>
// This brings in <chrono>, so this file can use its declarations.
#include <chrono>
// This brings in <mutex>, so this file can use its declarations.
#include <mutex>
// This brings in <sstream>, so this file can use its declarations.
#include <sstream>
// This brings in <iomanip>, so this file can use its declarations.
#include <iomanip>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// Session management: tokens map to usernames with expiry times.
// This is a source comment that explains nearby code.
// Tokens are server-side only; the frontend stores only the token string.

// This begins a reusable type or template: a blueprint for related data and behavior.
class SessionManager {
// This begins a reusable type or template: a blueprint for related data and behavior.
    struct Session {
// This creates or updates a value that the following logic uses.
        std::string username;
// This performs one step of the current operation, then continues to the next statement.
        std::chrono::steady_clock::time_point expires_at;
// This closes the block opened above, such as a function, class, or control-flow branch.
    };

// This creates or updates a value that the following logic uses.
    std::unordered_map<std::string, Session> sessions_;
// This performs one step of the current operation, then continues to the next statement.
    std::mutex mtx_;
// This is part of the current expression or block; read it together with the nearby lines.
    std::chrono::minutes ttl_{60}; // 1 hour session lifetime

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string generateToken() {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        static std::mt19937_64 rng(std::random_device{}());
// This performs one step of the current operation, then continues to the next statement.
        std::uniform_int_distribution<uint64_t> dist;
// This performs one step of the current operation, then continues to the next statement.
        std::ostringstream oss;
// This is part of the current expression or block; read it together with the nearby lines.
        oss << std::hex << std::setfill('0')
// This is part of the current expression or block; read it together with the nearby lines.
            << std::setw(16) << dist(rng)
// This performs one step of the current operation, then continues to the next statement.
            << std::setw(16) << dist(rng);
// This sends a result back to the code that called this function.
        return oss.str();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// The following members are available to other parts of the program.
public:
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string createSession(const std::string& username) {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This creates or updates a value that the following logic uses.
        std::string token = generateToken();
// This opens a scope for the related statements that follow.
        sessions_[token] = {
// This is part of the current expression or block; read it together with the nearby lines.
            username,
// This is part of the current expression or block; read it together with the nearby lines.
            std::chrono::steady_clock::now() + ttl_
// This closes the block opened above, such as a function, class, or control-flow branch.
        };
// This sends a result back to the code that called this function.
        return token;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string validate(const std::string& token) {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This creates or updates a value that the following logic uses.
        auto it = sessions_.find(token);
// This controls which part of the program runs next, based on the current condition or result.
        if (it == sessions_.end()) return "";
// This controls which part of the program runs next, based on the current condition or result.
        if (std::chrono::steady_clock::now() > it->second.expires_at) {
// This performs one step of the current operation, then continues to the next statement.
            sessions_.erase(it);
// This sends a result back to the code that called this function.
            return "";
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return it->second.username;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void invalidate(const std::string& token) {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        sessions_.erase(token);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // For testing: force a session to expire
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void expireSession(const std::string& token) {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::lock_guard lock(mtx_);
// This creates or updates a value that the following logic uses.
        auto it = sessions_.find(token);
// This controls which part of the program runs next, based on the current condition or result.
        if (it != sessions_.end()) {
// This is part of the current expression or block; read it together with the nearby lines.
            it->second.expires_at = std::chrono::steady_clock::now() -
// This performs one step of the current operation, then continues to the next statement.
                                    std::chrono::seconds(1);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
