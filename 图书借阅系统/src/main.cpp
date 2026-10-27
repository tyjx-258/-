#include "Library.h"

#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

std::string executableDirectory() {
#ifdef _WIN32
    char path[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameA(nullptr, path, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) {
        return ".";
    }

    const std::string fullPath(path, length);
    const std::size_t separator = fullPath.find_last_of("\\/");
    return separator == std::string::npos
               ? "."
               : fullPath.substr(0, separator);
#else
    return ".";
#endif
}

void showMenu() {
    std::cout << "\n===== Library System =====\n"
              << "1. Add book\n"
              << "2. Add reader\n"
              << "3. List books\n"
              << "4. Search books\n"
              << "5. Borrow book\n"
              << "6. Return book\n"
              << "7. List records\n"
              << "8. Save data\n"
              << "0. Save and exit\n"
              << "Choice: ";
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

int readInt(const std::string& prompt) {
    while (true) {
        const std::string value = readLine(prompt);
        try {
            std::size_t parsedLength = 0;
            const int result = std::stoi(value, &parsedLength);
            if (parsedLength == value.size()) {
                return result;
            }
        } catch (const std::exception&) {
        }
        std::cout << "Please enter a valid integer.\n";
    }
}

void printBooks(const std::vector<Book>& books) {
    if (books.empty()) {
        std::cout << "No books.\n";
        return;
    }
    for (const Book& book : books) {
        std::cout << book.isbn() << " | " << book.title() << " | "
                  << book.author() << " | available "
                  << book.availableCopies() << '/' << book.totalCopies()
                  << '\n';
    }
}

void printRecords(const std::vector<BorrowRecord>& records) {
    if (records.empty()) {
        std::cout << "No records.\n";
        return;
    }
    for (const BorrowRecord& record : records) {
        std::cout << record.recordId() << " | " << record.readerId() << " | "
                  << record.isbn() << " | due " << record.dueDate() << " | "
                  << (record.isReturned() ? "returned " + record.returnDate()
                                          : "borrowed")
                  << '\n';
    }
}

bool addBookFromInput(Library& library) {
    const std::string isbn = readLine("ISBN: ");
    const std::string title = readLine("Title: ");
    const std::string author = readLine("Author: ");
    const int copies = readInt("Total copies: ");

    std::string message;
    const bool ok = library.addBook(isbn, title, author, copies, message);
    std::cout << message << '\n';
    return ok;
}

bool addReaderFromInput(Library& library) {
    const std::string id = readLine("Reader id: ");
    const std::string name = readLine("Name: ");
    const std::string type = readLine("Type (STUDENT/TEACHER): ");

    std::string message;
    const bool ok = library.addReader(id, name, type, message);
    std::cout << message << '\n';
    return ok;
}

}  // namespace

int main() {
    Library library(executableDirectory() + "/data");
    std::string message;
    if (!library.load(message)) {
        std::cerr << "Load failed: " << message << '\n';
        return 1;
    }

    std::cout << "Loaded library data.\n";
    bool running = true;
    while (running) {
        showMenu();
        std::string choiceText;
        if (!std::getline(std::cin, choiceText)) {
            break;
        }

        try {
            const int choice = std::stoi(choiceText);
            switch (choice) {
                case 1:
                    addBookFromInput(library);
                    break;
                case 2:
                    addReaderFromInput(library);
                    break;
                case 3:
                    printBooks(library.books());
                    break;
                case 4: {
                    const std::string keyword = readLine("Keyword: ");
                    printBooks(library.searchBooks(keyword));
                    break;
                }
                case 5: {
                    const std::string readerId = readLine("Reader id: ");
                    const std::string isbn = readLine("ISBN: ");
                    library.borrowBook(readerId, isbn, message);
                    std::cout << message << '\n';
                    break;
                }
                case 6: {
                    const std::string readerId = readLine("Reader id: ");
                    const std::string isbn = readLine("ISBN: ");
                    library.returnBook(readerId, isbn, message);
                    std::cout << message << '\n';
                    break;
                }
                case 7:
                    printRecords(library.records());
                    break;
                case 8:
                    library.save(message);
                    std::cout << message << '\n';
                    break;
                case 0:
                    library.save(message);
                    std::cout << message << '\n';
                    running = false;
                    break;
                default:
                    std::cout << "Unknown option.\n";
                    break;
            }
        } catch (const std::exception&) {
            std::cout << "Invalid option.\n";
        }
    }
    return 0;
}
