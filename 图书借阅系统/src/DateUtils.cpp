#include "DateUtils.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {

const char* const kIsoDateFormat = "%Y-%m-%d";

std::string formatDate(const std::tm& date) {
    std::ostringstream stream;
    stream << std::put_time(&date, kIsoDateFormat);
    return stream.str();
}

std::tm parseDate(const std::string& isoDate) {
    std::tm date{};
    std::istringstream stream(isoDate);
    stream >> std::get_time(&date, kIsoDateFormat);
    if (stream.fail()) {
        throw std::invalid_argument("invalid ISO date");
    }
    return date;
}

}  // namespace

namespace DateUtils {

std::string todayIso() {
    const std::time_t now = std::time(nullptr);
    const std::tm* local = std::localtime(&now);
    if (!local) {
        throw std::runtime_error("cannot read local time");
    }
    return formatDate(*local);
}

std::string addDaysIso(const std::string& isoDate, int days) {
    std::tm date = parseDate(isoDate);
    date.tm_hour = 12;
    date.tm_min = 0;
    date.tm_sec = 0;

    const std::time_t base = std::mktime(&date);
    if (base == static_cast<std::time_t>(-1)) {
        throw std::invalid_argument("cannot convert ISO date");
    }

    const std::time_t shifted =
        base + static_cast<std::time_t>(days) * 24 * 60 * 60;
    const std::tm* result = std::localtime(&shifted);
    if (!result) {
        throw std::runtime_error("cannot format shifted date");
    }
    return formatDate(*result);
}

}  // namespace DateUtils
