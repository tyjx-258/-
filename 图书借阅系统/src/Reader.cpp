#include "Reader.h"

#include "CsvUtils.h"

#include <memory>
#include <stdexcept>

namespace {

std::unique_ptr<BorrowRule> makeRule(const std::string& readerType) {
    if (readerType == "STUDENT") {
        return std::make_unique<StudentBorrowRule>();
    }
    if (readerType == "TEACHER") {
        return std::make_unique<TeacherBorrowRule>();
    }
    return nullptr;
}

}  // namespace

Reader::Reader(std::string id,
               std::string name,
               std::unique_ptr<BorrowRule> rule)
    : id_(std::move(id)), name_(std::move(name)), rule_(std::move(rule)) {
    if (id_.empty() || name_.empty() || !rule_) {
        throw std::invalid_argument("invalid reader");
    }
}

const std::string& Reader::id() const {
    return id_;
}

const std::string& Reader::name() const {
    return name_;
}

const BorrowRule& Reader::rule() const {
    return *rule_;
}

bool Reader::canBorrow(std::size_t currentCount) const {
    return currentCount < rule_->maxBorrowCount();
}

std::string Reader::toCsv() const {
    return id_ + "," + name_ + "," + rule_->readerType();
}

std::optional<Reader> Reader::fromCsv(const std::string& line) {
    const std::vector<std::string> fields = CsvUtils::split(line);
    if (fields.size() != 3) {
        return std::nullopt;
    }

    const std::string id = CsvUtils::trim(fields[0]);
    const std::string name = CsvUtils::trim(fields[1]);
    const std::string type = CsvUtils::trim(fields[2]);
    std::unique_ptr<BorrowRule> rule = makeRule(type);
    if (id.empty() || name.empty() || !rule) {
        return std::nullopt;
    }

    return Reader(id, name, std::move(rule));
}
