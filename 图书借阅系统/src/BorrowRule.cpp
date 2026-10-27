#include "BorrowRule.h"

std::string StudentBorrowRule::readerType() const {
    return "STUDENT";
}

std::size_t StudentBorrowRule::maxBorrowCount() const {
    return 3;
}

int StudentBorrowRule::borrowDays() const {
    return 30;
}

std::string TeacherBorrowRule::readerType() const {
    return "TEACHER";
}

std::size_t TeacherBorrowRule::maxBorrowCount() const {
    return 8;
}

int TeacherBorrowRule::borrowDays() const {
    return 90;
}
