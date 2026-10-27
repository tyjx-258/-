#include "CsvUtils.h"

#include <cctype>

namespace CsvUtils {

std::string trim(const std::string& value) {
    std::size_t begin = 0;
    while (begin < value.size() &&
           std::isspace(static_cast<unsigned char>(value[begin]))) {
        ++begin;
    }

    std::size_t end = value.size();
    while (end > begin &&
           std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }
    return value.substr(begin, end - begin);
}

std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;

    for (char ch : line) {
        if (ch == ',') {
            fields.push_back(current);
            current.clear();
        } else {
            current.push_back(ch);
        }
    }
    fields.push_back(current);
    return fields;
}

bool containsComma(const std::string& value) {
    return value.find(',') != std::string::npos;
}

}  // namespace CsvUtils
