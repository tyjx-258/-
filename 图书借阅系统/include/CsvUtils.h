#pragma once

#include <string>
#include <vector>

namespace CsvUtils {

/** @return 去除字符串首尾空白后的副本。 */
std::string trim(const std::string& value);

/**
 * @brief 拆分不含转义逗号的 CSV 行。
 * @return 保留空字段的字段列表。
 */
std::vector<std::string> split(const std::string& line);

/** @return 字段是否包含项目 CSV 保留的逗号。 */
bool containsComma(const std::string& value);

}  // namespace CsvUtils
