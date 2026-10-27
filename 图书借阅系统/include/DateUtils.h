#pragma once

#include <string>

namespace DateUtils {

/** @return 本机当前日期，格式为 YYYY-MM-DD。 */
std::string todayIso();

/**
 * @brief 在 YYYY-MM-DD 日期上增加天数。
 * @throws std::invalid_argument 当日期格式错误时抛出。
 */
std::string addDaysIso(const std::string& isoDate, int days);

}  // namespace DateUtils
