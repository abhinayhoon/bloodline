// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/NotificationService.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <fstream>, so this file can use its declarations.
#include <fstream>
// This brings in <iostream>, so this file can use its declarations.
#include <iostream>
// This brings in <memory>, so this file can use its declarations.
#include <memory>
// This brings in <mutex>, so this file can use its declarations.
#include <mutex>
// This brings in "bloodline/DateUtil.hpp", so this file can use its declarations.
#include "bloodline/DateUtil.hpp"

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// Abstraction: pure virtual interface for notification delivery.
// This is a source comment that explains nearby code.
// Business logic (reminders, urgent matching) depends only on this interface.
// This begins a reusable type or template: a blueprint for related data and behavior.
class INotificationService {
// The following members are available to other parts of the program.
public:
// This performs one step of the current operation, then continues to the next statement.
    virtual ~INotificationService() = default;
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    virtual void send(const std::string& recipient,
// This creates or updates a value that the following logic uses.
                      const std::string& subject,
// This creates or updates a value that the following logic uses.
                      const std::string& message) = 0;
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// Concrete implementation: logs notifications to disk for audit and testing.
// This begins a reusable type or template: a blueprint for related data and behavior.
class LogNotificationService : public INotificationService {
// This creates or updates a value that the following logic uses.
    std::string logFilePath_;
// This performs one step of the current operation, then continues to the next statement.
    std::mutex mtx_;

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit LogNotificationService(const std::string& logFilePath = "data/notifications.log")
// This is part of the current expression or block; read it together with the nearby lines.
        : logFilePath_(logFilePath) {}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void send(const std::string& recipient,
// This creates or updates a value that the following logic uses.
              const std::string& subject,
// This creates or updates a value that the following logic uses.
              const std::string& message) override {
// This performs one step of the current operation, then continues to the next statement.
        std::lock_guard<std::mutex> lock(mtx_);
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::ofstream out(logFilePath_, std::ios::app);
// This controls which part of the program runs next, based on the current condition or result.
        if (out.is_open()) {
// This is part of the current expression or block; read it together with the nearby lines.
            out << "[" << DateUtil::today() << "] TO: " << recipient
// This is part of the current expression or block; read it together with the nearby lines.
                << " | SUBJECT: " << subject
// This performs one step of the current operation, then continues to the next statement.
                << " | MSG: " << message << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
