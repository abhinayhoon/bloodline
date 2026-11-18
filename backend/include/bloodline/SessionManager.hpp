#pragma once
#include <string>
#include <unordered_map>
#include <random>
#include <chrono>
#include <mutex>
#include <sstream>
#include <iomanip>

namespace bloodline {

// Session management: tokens map to usernames with expiry times.
// Tokens are server-side only; the frontend stores only the token string.

class SessionManager {
    struct Session {
        std::string username;
        std::chrono::steady_clock::time_point expires_at;
    };

    std::unordered_map<std::string, Session> sessions_;
    std::mutex mtx_;
    std::chrono::minutes ttl_{60}; // 1 hour session lifetime

    std::string generateToken() {
        static std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<uint64_t> dist;
        std::ostringstream oss;
        oss << std::hex << std::setfill('0')
            << std::setw(16) << dist(rng)
            << std::setw(16) << dist(rng);
        return oss.str();
    }

public:
    std::string createSession(const std::string& username) {
        std::lock_guard lock(mtx_);
        std::string token = generateToken();
        sessions_[token] = {
            username,
            std::chrono::steady_clock::now() + ttl_
        };
        return token;
    }

    std::string validate(const std::string& token) {
        std::lock_guard lock(mtx_);
        auto it = sessions_.find(token);
        if (it == sessions_.end()) return "";
        if (std::chrono::steady_clock::now() > it->second.expires_at) {
            sessions_.erase(it);
            return "";
        }
        return it->second.username;
    }

    void invalidate(const std::string& token) {
        std::lock_guard lock(mtx_);
        sessions_.erase(token);
    }

    // For testing: force a session to expire
    void expireSession(const std::string& token) {
        std::lock_guard lock(mtx_);
        auto it = sessions_.find(token);
        if (it != sessions_.end()) {
            it->second.expires_at = std::chrono::steady_clock::now() -
                                    std::chrono::seconds(1);
        }
    }
};

} // namespace bloodline
