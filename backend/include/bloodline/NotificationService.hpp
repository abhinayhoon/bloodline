#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include "bloodline/DateUtil.hpp"

namespace bloodline {

// Abstraction: pure virtual interface for notification delivery.
// Business logic (reminders, urgent matching) depends only on this interface.
class INotificationService {
public:
    virtual ~INotificationService() = default;
    virtual void send(const std::string& recipient,
                      const std::string& subject,
                      const std::string& message) = 0;
};

// Concrete implementation: logs notifications to disk for audit and testing.
class LogNotificationService : public INotificationService {
    std::string logFilePath_;
    std::mutex mtx_;

public:
    explicit LogNotificationService(const std::string& logFilePath = "data/notifications.log")
        : logFilePath_(logFilePath) {}

    void send(const std::string& recipient,
              const std::string& subject,
              const std::string& message) override {
        std::lock_guard<std::mutex> lock(mtx_);
        std::ofstream out(logFilePath_, std::ios::app);
        if (out.is_open()) {
            out << "[" << DateUtil::today() << "] TO: " << recipient
                << " | SUBJECT: " << subject
                << " | MSG: " << message << "\n";
        }
    }
};

} // namespace bloodline
