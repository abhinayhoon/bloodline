// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/DateUtil.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <ctime>, so this file can use its declarations.
#include <ctime>
// This brings in <iomanip>, so this file can use its declarations.
#include <iomanip>
// This brings in <sstream>, so this file can use its declarations.
#include <sstream>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This begins a reusable type or template: a blueprint for related data and behavior.
class DateUtil {
// The following members are available to other parts of the program.
public:
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    static bool isValid(const std::string& value) {
// This controls which part of the program runs next, based on the current condition or result.
        if (value.size() != 10 || value[4] != '-' || value[7] != '-') return false;
// This controls which part of the program runs next, based on the current condition or result.
        for (size_t i = 0; i < value.size(); ++i) if (i != 4 && i != 7 && (value[i] < '0' || value[i] > '9')) return false;
// This creates or updates a value that the following logic uses.
        int year = std::stoi(value.substr(0, 4)), month = std::stoi(value.substr(5, 2)), day = std::stoi(value.substr(8, 2));
// This controls which part of the program runs next, based on the current condition or result.
        if (year < 1900 || year > 2200 || month < 1 || month > 12) return false;
// This creates or updates a value that the following logic uses.
        int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
// This controls which part of the program runs next, based on the current condition or result.
        if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) days[1] = 29;
// This sends a result back to the code that called this function.
        return day >= 1 && day <= days[month - 1];
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    static std::string today() {
// This performs one step of the current operation, then continues to the next statement.
        std::time_t t = std::time(nullptr);
// This performs one step of the current operation, then continues to the next statement.
        std::tm* now = std::localtime(&t);
// This performs one step of the current operation, then continues to the next statement.
        char buf[32];
// This performs one step of the current operation, then continues to the next statement.
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", now);
// This sends a result back to the code that called this function.
        return std::string(buf);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    static std::string addDays(const std::string& dateStr, int days) {
// This creates or updates a value that the following logic uses.
        int y = 0, m = 0, d = 0;
// This controls which part of the program runs next, based on the current condition or result.
        if (std::sscanf(dateStr.c_str(), "%d-%d-%d", &y, &m, &d) != 3) {
// This sends a result back to the code that called this function.
            return dateStr;
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This performs one step of the current operation, then continues to the next statement.
        std::tm tm = {};
// This performs one step of the current operation, then continues to the next statement.
        tm.tm_year = y - 1900;
// This performs one step of the current operation, then continues to the next statement.
        tm.tm_mon = m - 1;
// This performs one step of the current operation, then continues to the next statement.
        tm.tm_mday = d + days;
// This performs one step of the current operation, then continues to the next statement.
        tm.tm_isdst = -1;

// This performs one step of the current operation, then continues to the next statement.
        std::time_t t = std::mktime(&tm);
// This controls which part of the program runs next, based on the current condition or result.
        if (t == -1) return dateStr;

// This performs one step of the current operation, then continues to the next statement.
        std::tm* res = std::localtime(&t);
// This performs one step of the current operation, then continues to the next statement.
        char buf[32];
// This performs one step of the current operation, then continues to the next statement.
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", res);
// This sends a result back to the code that called this function.
        return std::string(buf);
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Returns d2 - d1 in days
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    static int daysBetween(const std::string& d1, const std::string& d2) {
// This creates or updates a value that the following logic uses.
        int y1 = 0, m1 = 0, day1 = 0;
// This creates or updates a value that the following logic uses.
        int y2 = 0, m2 = 0, day2 = 0;
// This controls which part of the program runs next, based on the current condition or result.
        if (std::sscanf(d1.c_str(), "%d-%d-%d", &y1, &m1, &day1) != 3 ||
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
            std::sscanf(d2.c_str(), "%d-%d-%d", &y2, &m2, &day2) != 3) {
// This sends a result back to the code that called this function.
            return 0;
// This closes the block opened above, such as a function, class, or control-flow branch.
        }

// This performs one step of the current operation, then continues to the next statement.
        std::tm tm1 = {};
// This performs one step of the current operation, then continues to the next statement.
        tm1.tm_year = y1 - 1900;
// This performs one step of the current operation, then continues to the next statement.
        tm1.tm_mon = m1 - 1;
// This performs one step of the current operation, then continues to the next statement.
        tm1.tm_mday = day1;
// This performs one step of the current operation, then continues to the next statement.
        tm1.tm_isdst = -1;

// This performs one step of the current operation, then continues to the next statement.
        std::tm tm2 = {};
// This performs one step of the current operation, then continues to the next statement.
        tm2.tm_year = y2 - 1900;
// This performs one step of the current operation, then continues to the next statement.
        tm2.tm_mon = m2 - 1;
// This performs one step of the current operation, then continues to the next statement.
        tm2.tm_mday = day2;
// This performs one step of the current operation, then continues to the next statement.
        tm2.tm_isdst = -1;

// This performs one step of the current operation, then continues to the next statement.
        std::time_t t1 = std::mktime(&tm1);
// This performs one step of the current operation, then continues to the next statement.
        std::time_t t2 = std::mktime(&tm2);

// This creates or updates a value that the following logic uses.
        double seconds = std::difftime(t2, t1);
// This sends a result back to the code that called this function.
        return static_cast<int>(seconds / (60 * 60 * 24));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    static bool isBeforeOrEqual(const std::string& d1, const std::string& d2) {
// This sends a result back to the code that called this function.
        return d1 <= d2;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
