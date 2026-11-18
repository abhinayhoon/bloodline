#pragma once
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace bloodline {

class DateUtil {
public:
    static bool isValid(const std::string& value) {
        if (value.size() != 10 || value[4] != '-' || value[7] != '-') return false;
        for (size_t i = 0; i < value.size(); ++i) if (i != 4 && i != 7 && (value[i] < '0' || value[i] > '9')) return false;
        int year = std::stoi(value.substr(0, 4)), month = std::stoi(value.substr(5, 2)), day = std::stoi(value.substr(8, 2));
        if (year < 1900 || year > 2200 || month < 1 || month > 12) return false;
        int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) days[1] = 29;
        return day >= 1 && day <= days[month - 1];
    }
    static std::string today() {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", now);
        return std::string(buf);
    }

    static std::string addDays(const std::string& dateStr, int days) {
        int y = 0, m = 0, d = 0;
        if (std::sscanf(dateStr.c_str(), "%d-%d-%d", &y, &m, &d) != 3) {
            return dateStr;
        }
        std::tm tm = {};
        tm.tm_year = y - 1900;
        tm.tm_mon = m - 1;
        tm.tm_mday = d + days;
        tm.tm_isdst = -1;

        std::time_t t = std::mktime(&tm);
        if (t == -1) return dateStr;

        std::tm* res = std::localtime(&t);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", res);
        return std::string(buf);
    }

    // Returns d2 - d1 in days
    static int daysBetween(const std::string& d1, const std::string& d2) {
        int y1 = 0, m1 = 0, day1 = 0;
        int y2 = 0, m2 = 0, day2 = 0;
        if (std::sscanf(d1.c_str(), "%d-%d-%d", &y1, &m1, &day1) != 3 ||
            std::sscanf(d2.c_str(), "%d-%d-%d", &y2, &m2, &day2) != 3) {
            return 0;
        }

        std::tm tm1 = {};
        tm1.tm_year = y1 - 1900;
        tm1.tm_mon = m1 - 1;
        tm1.tm_mday = day1;
        tm1.tm_isdst = -1;

        std::tm tm2 = {};
        tm2.tm_year = y2 - 1900;
        tm2.tm_mon = m2 - 1;
        tm2.tm_mday = day2;
        tm2.tm_isdst = -1;

        std::time_t t1 = std::mktime(&tm1);
        std::time_t t2 = std::mktime(&tm2);

        double seconds = std::difftime(t2, t1);
        return static_cast<int>(seconds / (60 * 60 * 24));
    }

    static bool isBeforeOrEqual(const std::string& d1, const std::string& d2) {
        return d1 <= d2;
    }
};

} // namespace bloodline
