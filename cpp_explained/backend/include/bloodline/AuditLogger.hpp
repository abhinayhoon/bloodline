// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/AuditLogger.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <vector>, so this file can use its declarations.
#include <vector>
// This brings in <fstream>, so this file can use its declarations.
#include <fstream>
// This brings in <sstream>, so this file can use its declarations.
#include <sstream>
// This brings in <mutex>, so this file can use its declarations.
#include <mutex>
// This brings in <nlohmann/json.hpp>, so this file can use its declarations.
#include <nlohmann/json.hpp>
// This brings in "bloodline/DateUtil.hpp", so this file can use its declarations.
#include "bloodline/DateUtil.hpp"

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This begins a reusable type or template: a blueprint for related data and behavior.
struct AuditEntry {
// This creates or updates a value that the following logic uses.
    std::string id;
// This creates or updates a value that the following logic uses.
    std::string timestamp;
// This creates or updates a value that the following logic uses.
    std::string username;
// This creates or updates a value that the following logic uses.
    std::string action;
// This creates or updates a value that the following logic uses.
    std::string details;

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    nlohmann::json toJson() const {
// This sends a result back to the code that called this function.
        return {
// This is part of the current expression or block; read it together with the nearby lines.
            {"id", id},
// This is part of the current expression or block; read it together with the nearby lines.
            {"timestamp", timestamp},
// This is part of the current expression or block; read it together with the nearby lines.
            {"username", username},
// This is part of the current expression or block; read it together with the nearby lines.
            {"action", action},
// This is part of the current expression or block; read it together with the nearby lines.
            {"details", details}
// This closes the block opened above, such as a function, class, or control-flow branch.
        };
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::string toCsv() const {
// This is a source comment that explains nearby code.
        // Simple escape for commas
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        auto escape = [](const std::string& s) {
// This creates or updates a value that the following logic uses.
            std::string out = s;
// This performs one step of the current operation, then continues to the next statement.
            size_t pos = 0;
// This controls which part of the program runs next, based on the current condition or result.
            while ((pos = out.find(',', pos)) != std::string::npos) {
// This performs one step of the current operation, then continues to the next statement.
                out.replace(pos, 1, ";");
// This performs one step of the current operation, then continues to the next statement.
                pos++;
// This closes the block opened above, such as a function, class, or control-flow branch.
            }
// This sends a result back to the code that called this function.
            return out;
// This closes the block opened above, such as a function, class, or control-flow branch.
        };
// This sends a result back to the code that called this function.
        return id + "," + timestamp + "," + username + "," + action + "," + escape(details) + "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    static AuditEntry fromCsv(const std::string& line) {
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::stringstream ss(line);
// This creates or updates a value that the following logic uses.
        std::string id, ts, user, action, details;
// This performs one step of the current operation, then continues to the next statement.
        std::getline(ss, id, ',');
// This performs one step of the current operation, then continues to the next statement.
        std::getline(ss, ts, ',');
// This performs one step of the current operation, then continues to the next statement.
        std::getline(ss, user, ',');
// This performs one step of the current operation, then continues to the next statement.
        std::getline(ss, action, ',');
// This performs one step of the current operation, then continues to the next statement.
        std::getline(ss, details);
// This sends a result back to the code that called this function.
        return {id, ts, user, action, details};
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// Design Pattern: Observer / Event Recorder for audit compliance.
// This begins a reusable type or template: a blueprint for related data and behavior.
class AuditLogger {
// This creates or updates a value that the following logic uses.
    std::string filePath_;
// This creates or updates a value that the following logic uses.
    std::vector<AuditEntry> entries_;
// This performs one step of the current operation, then continues to the next statement.
    std::mutex mtx_;

// The following members are available to other parts of the program.
public:
// This creates or updates a value that the following logic uses.
    explicit AuditLogger(const std::string& filePath = "data/audit_log.csv")
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        : filePath_(filePath) {
// This performs one step of the current operation, then continues to the next statement.
        load();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void log(const std::string& username, const std::string& action, const std::string& details) {
// This performs one step of the current operation, then continues to the next statement.
        std::lock_guard<std::mutex> lock(mtx_);
// This creates or updates a value that the following logic uses.
        std::string id = "AUDIT-" + std::to_string(entries_.size() + 1);
// This creates or updates a value that the following logic uses.
        std::string ts = DateUtil::today();
// This performs one step of the current operation, then continues to the next statement.
        AuditEntry entry{id, ts, username, action, details};
// This performs one step of the current operation, then continues to the next statement.
        entries_.push_back(entry);

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::ofstream out(filePath_, std::ios::app);
// This controls which part of the program runs next, based on the current condition or result.
        if (out.is_open()) {
// This performs one step of the current operation, then continues to the next statement.
            out << entry.toCsv();
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    void load() {
// This performs one step of the current operation, then continues to the next statement.
        std::lock_guard<std::mutex> lock(mtx_);
// This performs one step of the current operation, then continues to the next statement.
        entries_.clear();
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        std::ifstream in(filePath_);
// This controls which part of the program runs next, based on the current condition or result.
        if (!in.is_open()) return;
// This creates or updates a value that the following logic uses.
        std::string line;
// This controls which part of the program runs next, based on the current condition or result.
        while (std::getline(in, line)) {
// This controls which part of the program runs next, based on the current condition or result.
            if (line.empty()) continue;
// This performs one step of the current operation, then continues to the next statement.
            entries_.push_back(AuditEntry::fromCsv(line));
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    std::vector<AuditEntry> all() {
// This performs one step of the current operation, then continues to the next statement.
        std::lock_guard<std::mutex> lock(mtx_);
// This sends a result back to the code that called this function.
        return entries_;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
