#pragma once

#include <optional>
#include <string>

/**
 * @brief 一次借阅操作形成的记录。
 */
class BorrowRecord {
public:
    BorrowRecord(int recordId,
                 std::string readerId,
                 std::string isbn,
                 std::string borrowDate,
                 std::string dueDate,
                 std::string returnDate = "");

    /** @return 记录编号。 */
    int recordId() const;

    /** @return 读者编号。 */
    const std::string& readerId() const;

    /** @return ISBN。 */
    const std::string& isbn() const;

    /** @return 借出日期。 */
    const std::string& borrowDate() const;

    /** @return 应还日期。 */
    const std::string& dueDate() const;

    /** @return 归还日期，未归还时为空字符串。 */
    const std::string& returnDate() const;

    /** @return 是否已归还。 */
    bool isReturned() const;

    /** @brief 标记为已归还。 */
    void markReturned(const std::string& date);

    /** @return CSV 行。 */
    std::string toCsv() const;

    /**
     * @brief 从 CSV 行解析借阅记录。
     * @return 成功时返回记录，格式错误时返回 std::nullopt。
     */
    static std::optional<BorrowRecord> fromCsv(const std::string& line);

private:
    int recordId_;
    std::string readerId_;
    std::string isbn_;
    std::string borrowDate_;
    std::string dueDate_;
    std::string returnDate_;
};
