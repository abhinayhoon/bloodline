// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/Errors.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in <stdexcept>, so this file can use its declarations.
#include <stdexcept>
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <unordered_map>, so this file can use its declarations.
#include <unordered_map>
// This brings in <chrono>, so this file can use its declarations.
#include <chrono>
// This brings in <mutex>, so this file can use its declarations.
#include <mutex>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// Custom exception hierarchy mapped to HTTP status codes in one place.
// This is a source comment that explains nearby code.
// All domain errors derive from AppError so route handlers catch them uniformly.

// This begins a reusable type or template: a blueprint for related data and behavior.
class AppError : public std::runtime_error {
// This creates or updates a value that the following logic uses.
    int status_code_;
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    AppError(int status, const std::string& msg)
// This is part of the current expression or block; read it together with the nearby lines.
        : std::runtime_error(msg), status_code_(status) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int statusCode() const noexcept { return status_code_; }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This begins a reusable type or template: a blueprint for related data and behavior.
class BadRequestError : public AppError {
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit BadRequestError(const std::string& msg)
// This is part of the current expression or block; read it together with the nearby lines.
        : AppError(400, msg) {}
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This begins a reusable type or template: a blueprint for related data and behavior.
class UnauthorizedError : public AppError {
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit UnauthorizedError(const std::string& msg = "Unauthorized")
// This is part of the current expression or block; read it together with the nearby lines.
        : AppError(401, msg) {}
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This begins a reusable type or template: a blueprint for related data and behavior.
class ForbiddenError : public AppError {
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit ForbiddenError(const std::string& msg = "Forbidden")
// This is part of the current expression or block; read it together with the nearby lines.
        : AppError(403, msg) {}
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This begins a reusable type or template: a blueprint for related data and behavior.
class NotFoundError : public AppError {
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit NotFoundError(const std::string& msg = "Not found")
// This is part of the current expression or block; read it together with the nearby lines.
        : AppError(404, msg) {}
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This begins a reusable type or template: a blueprint for related data and behavior.
class ConflictError : public AppError {
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit ConflictError(const std::string& msg)
// This is part of the current expression or block; read it together with the nearby lines.
        : AppError(409, msg) {}
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This begins a reusable type or template: a blueprint for related data and behavior.
class TooManyRequestsError : public AppError {
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit TooManyRequestsError(const std::string& msg = "Too many attempts. Account locked temporarily.")
// This is part of the current expression or block; read it together with the nearby lines.
        : AppError(429, msg) {}
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This begins a reusable type or template: a blueprint for related data and behavior.
class InvalidStateTransitionError : public BadRequestError {
// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit InvalidStateTransitionError(const std::string& msg)
// This is part of the current expression or block; read it together with the nearby lines.
        : BadRequestError("Invalid state transition: " + msg) {}
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// RateLimiter for login attempts: 5 failed attempts locks for 5 minutes (300s)
// This begins a reusable type or template: a blueprint for related data and behavior.
class RateLimiter {
// This begins a reusable type or template: a blueprint for related data and behavior.
    struct AttemptInfo {
// This creates or updates a value that the following logic uses.
        int failedCount = 0;
// This performs one step of the current operation, then continues to the next statement.
        std::chrono::steady_clock::time_point lockExpiry;
// This closes the block opened above, such as a function, class, or control-flow branch.
    };
// This performs one step of the current operation, then continues to the next statement.
    std::mutex mtx_;
// This creates or updates a value that the following logic uses.
    std::unordered_map<std::string, AttemptInfo> records_;
// This creates or updates a value that the following logic uses.
    int maxAttempts_;
// This performs one step of the current operation, then continues to the next statement.
    std::chrono::seconds lockDuration_;

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    RateLimiter(int maxAttempts = 5, int lockSeconds = 300)
// This is part of the current expression or block; read it together with the nearby lines.
        : maxAttempts_(maxAttempts), lockDuration_(lockSeconds) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    bool isLocked(const std::string& key) {
// This performs one step of the current operation, then continues to the next statement.
        std::lock_guard<std::mutex> lock(mtx_);
// This creates or updates a value that the following logic uses.
        auto it = records_.find(key);
// This controls which part of the program runs next, based on the current condition or result.
        if (it == records_.end()) return false;
// This creates or updates a value that the following logic uses.
        auto now = std::chrono::steady_clock::now();
// This controls which part of the program runs next, based on the current condition or result.
        if (it->second.failedCount >= maxAttempts_) {
// This controls which part of the program runs next, based on the current condition or result.
            if (now < it->second.lockExpiry) {
// This sends a result back to the code that called this function.
                return true;
// This closes the block opened above, such as a function, class, or control-flow branch.
            } else {
// This is a source comment that explains nearby code.
                // Lock has expired, reset counter
// This performs one step of the current operation, then continues to the next statement.
                records_.erase(it);
// This sends a result back to the code that called this function.
                return false;
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return false;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    int remainingLockSeconds(const std::string& key) {
// This performs one step of the current operation, then continues to the next statement.
        std::lock_guard<std::mutex> lock(mtx_);
// This creates or updates a value that the following logic uses.
        auto it = records_.find(key);
// This controls which part of the program runs next, based on the current condition or result.
        if (it == records_.end()) return 0;
// This creates or updates a value that the following logic uses.
        auto now = std::chrono::steady_clock::now();
// This controls which part of the program runs next, based on the current condition or result.
        if (it->second.failedCount >= maxAttempts_ && now < it->second.lockExpiry) {
// This creates or updates a value that the following logic uses.
            auto diff = std::chrono::duration_cast<std::chrono::seconds>(it->second.lockExpiry - now).count();
// This sends a result back to the code that called this function.
            return static_cast<int>(diff);
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This sends a result back to the code that called this function.
        return 0;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void recordFailure(const std::string& key) {
// This performs one step of the current operation, then continues to the next statement.
        std::lock_guard<std::mutex> lock(mtx_);
// This creates or updates a value that the following logic uses.
        auto& info = records_[key];
// This performs one step of the current operation, then continues to the next statement.
        info.failedCount++;
// This controls which part of the program runs next, based on the current condition or result.
        if (info.failedCount >= maxAttempts_) {
// This performs one step of the current operation, then continues to the next statement.
            info.lockExpiry = std::chrono::steady_clock::now() + lockDuration_;
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void recordSuccess(const std::string& key) {
// This performs one step of the current operation, then continues to the next statement.
        std::lock_guard<std::mutex> lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        records_.erase(key);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void reset() {
// This performs one step of the current operation, then continues to the next statement.
        std::lock_guard<std::mutex> lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        records_.clear();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
