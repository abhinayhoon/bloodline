#pragma once
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <chrono>
#include <mutex>

namespace bloodline {

// Custom exception hierarchy mapped to HTTP status codes in one place.
// All domain errors derive from AppError so route handlers catch them uniformly.

class AppError : public std::runtime_error {
    int status_code_;
public:
    AppError(int status, const std::string& msg)
        : std::runtime_error(msg), status_code_(status) {}

    int statusCode() const noexcept { return status_code_; }
};

class BadRequestError : public AppError {
public:
    explicit BadRequestError(const std::string& msg)
        : AppError(400, msg) {}
};

class UnauthorizedError : public AppError {
public:
    explicit UnauthorizedError(const std::string& msg = "Unauthorized")
        : AppError(401, msg) {}
};

class ForbiddenError : public AppError {
public:
    explicit ForbiddenError(const std::string& msg = "Forbidden")
        : AppError(403, msg) {}
};

class NotFoundError : public AppError {
public:
    explicit NotFoundError(const std::string& msg = "Not found")
        : AppError(404, msg) {}
};

class ConflictError : public AppError {
public:
    explicit ConflictError(const std::string& msg)
        : AppError(409, msg) {}
};

class TooManyRequestsError : public AppError {
public:
    explicit TooManyRequestsError(const std::string& msg = "Too many attempts. Account locked temporarily.")
        : AppError(429, msg) {}
};

class InvalidStateTransitionError : public BadRequestError {
public:
    explicit InvalidStateTransitionError(const std::string& msg)
        : BadRequestError("Invalid state transition: " + msg) {}
};

// RateLimiter for login attempts: 5 failed attempts locks for 5 minutes (300s)
class RateLimiter {
    struct AttemptInfo {
        int failedCount = 0;
        std::chrono::steady_clock::time_point lockExpiry;
    };
    std::mutex mtx_;
    std::unordered_map<std::string, AttemptInfo> records_;
    int maxAttempts_;
    std::chrono::seconds lockDuration_;

public:
    RateLimiter(int maxAttempts = 5, int lockSeconds = 300)
        : maxAttempts_(maxAttempts), lockDuration_(lockSeconds) {}

    bool isLocked(const std::string& key) {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = records_.find(key);
        if (it == records_.end()) return false;
        auto now = std::chrono::steady_clock::now();
        if (it->second.failedCount >= maxAttempts_) {
            if (now < it->second.lockExpiry) {
                return true;
            } else {
                // Lock has expired, reset counter
                records_.erase(it);
                return false;
            }
        }
        return false;
    }

    int remainingLockSeconds(const std::string& key) {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = records_.find(key);
        if (it == records_.end()) return 0;
        auto now = std::chrono::steady_clock::now();
        if (it->second.failedCount >= maxAttempts_ && now < it->second.lockExpiry) {
            auto diff = std::chrono::duration_cast<std::chrono::seconds>(it->second.lockExpiry - now).count();
            return static_cast<int>(diff);
        }
        return 0;
    }

    void recordFailure(const std::string& key) {
        std::lock_guard<std::mutex> lock(mtx_);
        auto& info = records_[key];
        info.failedCount++;
        if (info.failedCount >= maxAttempts_) {
            info.lockExpiry = std::chrono::steady_clock::now() + lockDuration_;
        }
    }

    void recordSuccess(const std::string& key) {
        std::lock_guard<std::mutex> lock(mtx_);
        records_.erase(key);
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mtx_);
        records_.clear();
    }
};

} // namespace bloodline
