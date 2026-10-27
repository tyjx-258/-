#include "BorrowRecord.h"

#include "CsvUtils.h"

namespace {

std::optional<int> parseRecordId(const std::string& value) {
    try {
        std::size_t parsedLength = 0;
        const int result = std::stoi(value, &parsedLength);
        if (parsedLength != value.size() || result <= 0) {
            return std::nullopt;
        }
        return result;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

}  // namespace

BorrowRecord::BorrowRecord(int recordId,
                           std::string readerId,
                           std::string isbn,
                           std::string borrowDate,
                           std::string dueDate,
                           std::string returnDate)
    : recordId_(recordId),
      readerId_(std::move(readerId)),
      isbn_(std::move(isbn)),
      borrowDate_(std::move(borrowDate)),
      dueDate_(std::move(dueDate)),
      returnDate_(std::move(returnDate)) {
}

int BorrowRecord::recordId() const {
    return recordId_;
}

const std::string& BorrowRecord::readerId() const {
    return readerId_;
}

const std::string& BorrowRecord::isbn() const {
    return isbn_;
}

const std::string& BorrowRecord::borrowDate() const {
    return borrowDate_;
}

const std::string& BorrowRecord::dueDate() const {
    return dueDate_;
}

const std::string& BorrowRecord::returnDate() const {
    return returnDate_;
}

bool BorrowRecord::isReturned() const {
    return !returnDate_.empty();
}

void BorrowRecord::markReturned(const std::string& date) {
    returnDate_ = date;
}

std::string BorrowRecord::toCsv() const {
    return std::to_string(recordId_) + "," + readerId_ + "," + isbn_ + "," +
           borrowDate_ + "," + dueDate_ + "," + returnDate_;
}

std::optional<BorrowRecord> BorrowRecord::fromCsv(const std::string& line) {
    const std::vector<std::string> fields = CsvUtils::split(line);
    if (fields.size() != 6) {
        return std::nullopt;
    }

    const std::optional<int> recordId =
        parseRecordId(CsvUtils::trim(fields[0]));
    const std::string readerId = CsvUtils::trim(fields[1]);
    const std::string isbn = CsvUtils::trim(fields[2]);
    const std::string borrowDate = CsvUtils::trim(fields[3]);
    const std::string dueDate = CsvUtils::trim(fields[4]);
    const std::string returnDate = CsvUtils::trim(fields[5]);

    if (!recordId || readerId.empty() || isbn.empty() ||
        borrowDate.empty() || dueDate.empty()) {
        return std::nullopt;
    }

    return BorrowRecord(*recordId,
                        readerId,
                        isbn,
                        borrowDate,
                        dueDate,
                        returnDate);
}
