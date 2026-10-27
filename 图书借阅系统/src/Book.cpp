#include "Book.h"

#include "CsvUtils.h"

#include <stdexcept>

namespace {

std::optional<int> parseInt(const std::string& value) {
    try {
        std::size_t parsedLength = 0;
        const int result = std::stoi(value, &parsedLength);
        if (parsedLength != value.size()) {
            return std::nullopt;
        }
        return result;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

}  // namespace

Book::Book(std::string isbn,
           std::string title,
           std::string author,
           int totalCopies,
           int availableCopies)
    : isbn_(std::move(isbn)),
      title_(std::move(title)),
      author_(std::move(author)),
      totalCopies_(totalCopies),
      availableCopies_(availableCopies) {
    if (isbn_.empty() || title_.empty() || author_.empty()) {
        throw std::invalid_argument("book text fields cannot be empty");
    }
    if (totalCopies_ < 0 ||
        availableCopies_ < 0 ||
        availableCopies_ > totalCopies_) {
        throw std::invalid_argument("invalid book copy count");
    }
}

const std::string& Book::isbn() const {
    return isbn_;
}

const std::string& Book::title() const {
    return title_;
}

const std::string& Book::author() const {
    return author_;
}

int Book::totalCopies() const {
    return totalCopies_;
}

int Book::availableCopies() const {
    return availableCopies_;
}

bool Book::isAvailable() const {
    return availableCopies_ > 0;
}

bool Book::borrowCopy() {
    if (!isAvailable()) {
        return false;
    }
    --availableCopies_;
    return true;
}

bool Book::returnCopy() {
    if (availableCopies_ >= totalCopies_) {
        return false;
    }
    ++availableCopies_;
    return true;
}

std::string Book::toCsv() const {
    return isbn_ + "," + title_ + "," + author_ + "," +
           std::to_string(totalCopies_) + "," +
           std::to_string(availableCopies_);
}

std::optional<Book> Book::fromCsv(const std::string& line) {
    const std::vector<std::string> fields = CsvUtils::split(line);
    if (fields.size() != 5) {
        return std::nullopt;
    }

    const std::optional<int> total = parseInt(CsvUtils::trim(fields[3]));
    const std::optional<int> available = parseInt(CsvUtils::trim(fields[4]));
    if (!total || !available || *total < 0 || *available < 0 ||
        *available > *total) {
        return std::nullopt;
    }

    try {
        return Book(CsvUtils::trim(fields[0]),
                    CsvUtils::trim(fields[1]),
                    CsvUtils::trim(fields[2]),
                    *total,
                    *available);
    } catch (const std::invalid_argument&) {
        return std::nullopt;
    }
}
