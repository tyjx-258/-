#include "Library.h"

#include <cstdio>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

const char* const kTestDir = "tests";

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("FAILED: " + message);
    }
}

void removeTestData() {
    std::remove("tests/books.csv");
    std::remove("tests/readers.csv");
    std::remove("tests/records.csv");
}

void testBookAndReaderRules() {
    Book book("ISBN-1", "C++ Primer", "Lippman", 2, 2);
    require(book.isAvailable(), "new book should be available");
    require(book.borrowCopy(), "first borrow should succeed");
    require(book.availableCopies() == 1, "available copy count should decrease");
    require(book.returnCopy(), "return should succeed");
    require(book.availableCopies() == 2, "available copy count should recover");

    Reader student("S001", "Student", std::make_unique<StudentBorrowRule>());
    Reader teacher("T001", "Teacher", std::make_unique<TeacherBorrowRule>());
    require(student.canBorrow(2), "student should borrow fewer than 3 books");
    require(!student.canBorrow(3), "student should stop at 3 books");
    require(teacher.canBorrow(7), "teacher should borrow fewer than 8 books");
    require(!teacher.canBorrow(8), "teacher should stop at 8 books");
}

void testBorrowReturnAndSearch() {
    removeTestData();
    Library library(kTestDir);
    std::string message;

    require(library.addBook("ISBN-1", "C++ Primer", "Lippman", 1, message),
            "add book should succeed");
    require(!library.addBook("ISBN-1", "Duplicate", "Author", 1, message),
            "duplicate ISBN should fail");
    require(library.addReader("S001", "Student", "STUDENT", message),
            "add student should succeed");
    require(library.addReader("T001", "Teacher", "TEACHER", message),
            "add teacher should succeed");
    require(library.borrowBook("S001", "ISBN-1", message),
            "borrow should succeed");
    require(!library.borrowBook("S001", "ISBN-1", message),
            "borrow without copy should fail");
    require(library.searchBooks("Primer").size() == 1,
            "title search should find one book");
    require(library.returnBook("S001", "ISBN-1", message),
            "return should succeed");
    require(!library.returnBook("S001", "ISBN-1", message),
            "second return should fail");
    require(library.save(message), "save should succeed");
}

void testPersistence() {
    Library library(kTestDir);
    std::string message;
    require(library.load(message), "load should succeed");
    require(library.books().size() == 1, "one book should be loaded");
    require(library.readers().size() == 2, "two readers should be loaded");
    require(library.records().size() == 1, "one record should be loaded");
    require(library.records()[0].isReturned(), "record should be returned");
}

}  // namespace

int main() {
    try {
        testBookAndReaderRules();
        testBorrowReturnAndSearch();
        testPersistence();
        removeTestData();
        std::cout << "All library tests passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
