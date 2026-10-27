#pragma once

#include <cstddef>
#include <string>

/**
 * @brief 读者借阅规则抽象基类。
 */
class BorrowRule {
public:
    virtual ~BorrowRule() = default;

    /** @return 读者类型标识。 */
    virtual std::string readerType() const = 0;

    /** @return 同时可借图书的最大数量。 */
    virtual std::size_t maxBorrowCount() const = 0;

    /** @return 借阅天数。 */
    virtual int borrowDays() const = 0;
};

/**
 * @brief 学生借阅规则。
 */
class StudentBorrowRule final : public BorrowRule {
public:
    std::string readerType() const override;
    std::size_t maxBorrowCount() const override;
    int borrowDays() const override;
};

/**
 * @brief 教师借阅规则。
 */
class TeacherBorrowRule final : public BorrowRule {
public:
    std::string readerType() const override;
    std::size_t maxBorrowCount() const override;
    int borrowDays() const override;
};
