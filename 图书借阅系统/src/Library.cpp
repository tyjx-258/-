#include "Library.h"

#include "CsvUtils.h"
#include "DateUtils.h"

#include <algorithm>
#include <fstream>
#include <memory>
#include <stdexcept>

namespace {

const char* const kBookHeader = "isbn,title,author,total,available";
const char* const kReaderHeader = "id,name,type";
const char* const kRecordHeader =
    "recordId,readerId,isbn,borrowDate,dueDate,returnDate";

bool hasBlankOrComma(const std::string& value) {
    return value.empty() || CsvUtils::containsComma(value);
}

}  // namespace

Library::Library(std::string dataDir) : dataDir_(std::move(dataDir)) {
}

bool Library::load(std::string& message) {
    books_.clear();
    readers_.clear();
    records_.clear();

    if (!loadBooks(message) || !loadReaders(message) || !loadRecords(message)) {
        return false;
    }

    message = "data loaded";
    return true;
}

bool Library::loadBooks(std::string& message) {
    const std::string path = joinPath("books.csv");
    std::ifstream file(path);
    if (!file) {
        message = "cannot open " + path;
        return false;
    }

    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        if (CsvUtils::trim(line).empty()) {
            continue;
        }
        const std::optional<Book> book = Book::fromCsv(line);
        if (!book) {
            message = "invalid books.csv line: " + line;
            return false;
        }
        books_.push_back(*book);
    }
    return true;
}

bool Library::loadReaders(std::string& message) {
    const std::string path = joinPath("readers.csv");
    std::ifstream file(path);
    if (!file) {
        message = "cannot open " + path;
        return false;
    }

    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        if (CsvUtils::trim(line).empty()) {
            continue;
        }
        std::optional<Reader> reader = Reader::fromCsv(line);
        if (!reader) {
            message = "invalid readers.csv line: " + line;
            return false;
        }
        readers_.push_back(std::move(*reader));
    }
    return true;
}

bool Library::loadRecords(std::string& message) {
    const std::string path = joinPath("records.csv");
    std::ifstream file(path);
    if (!file) {
        message = "cannot open " + path;
        return false;
    }

    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        if (CsvUtils::trim(line).empty()) {
            continue;
        }
        const std::optional<BorrowRecord> record = BorrowRecord::fromCsv(line);
        if (!record) {
            message = "invalid records.csv line: " + line;
            return false;
        }
        records_.push_back(*record);
    }
    return true;
}

bool Library::save(std::string& message) const {
    std::ofstream books(joinPath("books.csv"), std::ios::trunc);
    std::ofstream readers(joinPath("readers.csv"), std::ios::trunc);
    std::ofstream records(joinPath("records.csv"), std::ios::trunc);
    if (!books || !readers || !records) {
        message = "cannot open data files for writing";
        return false;
    }

    books << kBookHeader << '\n';
    for (const Book& book : books_) {
        books << book.toCsv() << '\n';
    }

    readers << kReaderHeader << '\n';
    for (const Reader& reader : readers_) {
        readers << reader.toCsv() << '\n';
    }

    records << kRecordHeader << '\n';
    for (const BorrowRecord& record : records_) {
        records << record.toCsv() << '\n';
    }

    if (!books || !readers || !records) {
        message = "failed while writing data files";
        return false;
    }

    message = "data saved";
    return true;
}

bool Library::addBook(const std::string& isbn,
                      const std::string& title,
                      const std::string& author,
                      int totalCopies,
                      std::string& message) {
    if (hasBlankOrComma(isbn) || hasBlankOrComma(title) ||
        hasBlankOrComma(author)) {
        message = "book fields cannot be empty or contain commas";
        return false;
    }
    if (findBook(isbn)) {
        message = "duplicate ISBN";
        return false;
    }
    if (totalCopies <= 0) {
        message = "total copies must be positive";
        return false;
    }

    // Modification: validate all preconditions before changing book state.
    try {
        books_.emplace_back(isbn, title, author, totalCopies, totalCopies);
    } catch (const std::invalid_argument& error) {
        message = error.what();
        return false;
    }

    message = "book added";
    return true;
}

bool Library::addReader(const std::string& readerId,
                        const std::string& name,
                        const std::string& readerType,
                        std::string& message) {
    if (hasBlankOrComma(readerId) || hasBlankOrComma(name)) {
        message = "reader fields cannot be empty or contain commas";
        return false;
    }
    if (findReader(readerId)) {
        message = "duplicate reader id";
        return false;
    }

    std::unique_ptr<BorrowRule> rule;
    if (readerType == "STUDENT") {
        rule = std::make_unique<StudentBorrowRule>();
    } else if (readerType == "TEACHER") {
        rule = std::make_unique<TeacherBorrowRule>();
    } else {
        message = "reader type must be STUDENT or TEACHER";
        return false;
    }

    readers_.emplace_back(readerId, name, std::move(rule));
    message = "reader added";
    return true;
}

bool Library::borrowBook(const std::string& readerId,
                         const std::string& isbn,
                         std::string& message) {
    Book* book = findBookMutable(isbn);
    Reader* reader = findReaderMutable(readerId);
    if (!book) {
        message = "book not found";
        return false;
    }
    if (!reader) {
        message = "reader not found";
        return false;
    }
    if (!book->isAvailable()) {
        message = "no available copy";
        return false;
    }
    if (!reader->canBorrow(activeBorrowCount(readerId))) {
        message = "reader reached borrow limit";
        return false;
    }

    try {
        const std::string borrowDate = DateUtils::todayIso();
        const std::string dueDate =
            DateUtils::addDaysIso(borrowDate, reader->rule().borrowDays());
        book->borrowCopy();
        records_.emplace_back(nextRecordId(),
                              readerId,
                              isbn,
                              borrowDate,
                              dueDate,
                              "");
    } catch (const std::exception& error) {
        message = error.what();
        return false;
    }

    message = "book borrowed";
    return true;
}

bool Library::returnBook(const std::string& readerId,
                         const std::string& isbn,
                         std::string& message) {
    Book* book = findBookMutable(isbn);
    if (!book) {
        message = "book not found";
        return false;
    }

    for (auto record = records_.rbegin(); record != records_.rend(); ++record) {
        if (record->readerId() == readerId && record->isbn() == isbn &&
            !record->isReturned()) {
            if (!book->returnCopy()) {
                message = "book copy state is inconsistent";
                return false;
            }
            record->markReturned(DateUtils::todayIso());
            message = "book returned";
            return true;
        }
    }

    message = "active borrow record not found";
    return false;
}

std::vector<Book> Library::searchBooks(const std::string& keyword) const {
    std::vector<Book> result;
    // Modification: return copies so callers cannot mutate the owned books.
    std::copy_if(books_.begin(),
                 books_.end(),
                 std::back_inserter(result),
                 [&keyword](const Book& book) {
                     return book.isbn().find(keyword) != std::string::npos ||
                            book.title().find(keyword) != std::string::npos;
                 });
    return result;
}

const std::vector<Book>& Library::books() const {
    return books_;
}

const std::vector<Reader>& Library::readers() const {
    return readers_;
}

const std::vector<BorrowRecord>& Library::records() const {
    return records_;
}

Book* Library::findBookMutable(const std::string& isbn) {
    for (Book& book : books_) {
        if (book.isbn() == isbn) {
            return &book;
        }
    }
    return nullptr;
}

Reader* Library::findReaderMutable(const std::string& readerId) {
    for (Reader& reader : readers_) {
        if (reader.id() == readerId) {
            return &reader;
        }
    }
    return nullptr;
}

const Book* Library::findBook(const std::string& isbn) const {
    for (const Book& book : books_) {
        if (book.isbn() == isbn) {
            return &book;
        }
    }
    return nullptr;
}

const Reader* Library::findReader(const std::string& readerId) const {
    for (const Reader& reader : readers_) {
        if (reader.id() == readerId) {
            return &reader;
        }
    }
    return nullptr;
}

std::size_t Library::activeBorrowCount(const std::string& readerId) const {
    return static_cast<std::size_t>(std::count_if(
        records_.begin(),
        records_.end(),
        [&readerId](const BorrowRecord& record) {
            return record.readerId() == readerId && !record.isReturned();
        }));
}

int Library::nextRecordId() const {
    int next = 1;
    for (const BorrowRecord& record : records_) {
        next = std::max(next, record.recordId() + 1);
    }
    return next;
}

std::string Library::joinPath(const std::string& fileName) const {
    return dataDir_ + "/" + fileName;
}
