#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <mutex>
#include <nlohmann/json.hpp>
#include "bloodline/DateUtil.hpp"

namespace bloodline {

struct AuditEntry {
    std::string id;
    std::string timestamp;
    std::string username;
    std::string action;
    std::string details;

    nlohmann::json toJson() const {
        return {
            {"id", id},
            {"timestamp", timestamp},
            {"username", username},
            {"action", action},
            {"details", details}
        };
    }

    std::string toCsv() const {
        // Simple escape for commas
        auto escape = [](const std::string& s) {
            std::string out = s;
            size_t pos = 0;
            while ((pos = out.find(',', pos)) != std::string::npos) {
                out.replace(pos, 1, ";");
                pos++;
            }
            return out;
        };
        return id + "," + timestamp + "," + username + "," + action + "," + escape(details) + "\n";
    }

    static AuditEntry fromCsv(const std::string& line) {
        std::stringstream ss(line);
        std::string id, ts, user, action, details;
        std::getline(ss, id, ',');
        std::getline(ss, ts, ',');
        std::getline(ss, user, ',');
        std::getline(ss, action, ',');
        std::getline(ss, details);
        return {id, ts, user, action, details};
    }
};

// Design Pattern: Observer / Event Recorder for audit compliance.
class AuditLogger {
    std::string filePath_;
    std::vector<AuditEntry> entries_;
    std::mutex mtx_;

public:
    explicit AuditLogger(const std::string& filePath = "data/audit_log.csv")
        : filePath_(filePath) {
        load();
    }

    void log(const std::string& username, const std::string& action, const std::string& details) {
        std::lock_guard<std::mutex> lock(mtx_);
        std::string id = "AUDIT-" + std::to_string(entries_.size() + 1);
        std::string ts = DateUtil::today();
        AuditEntry entry{id, ts, username, action, details};
        entries_.push_back(entry);

        std::ofstream out(filePath_, std::ios::app);
        if (out.is_open()) {
            out << entry.toCsv();
        }
    }

    void load() {
        std::lock_guard<std::mutex> lock(mtx_);
        entries_.clear();
        std::ifstream in(filePath_);
        if (!in.is_open()) return;
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            entries_.push_back(AuditEntry::fromCsv(line));
        }
    }

    std::vector<AuditEntry> all() {
        std::lock_guard<std::mutex> lock(mtx_);
        return entries_;
    }
};

} // namespace bloodline
