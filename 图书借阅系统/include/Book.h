#pragma once

#include <optional>
#include <string>

/**
 * @brief 图书实体，负责自身副本状态管理。
 */
class Book {
public:
    /**
     * @brief 创建图书。
     * @throws std::invalid_argument 当副本数量不合法或文本为空时抛出。
     */
    Book(std::string isbn,
         std::string title,
         std::string author,
         int totalCopies,
         int availableCopies);

    /** @return ISBN。 */
    const std::string& isbn() const;

    /** @return 书名。 */
    const std::string& title() const;

    /** @return 作者。 */
    const std::string& author() const;

    /** @return 馆藏总数。 */
    int totalCopies() const;

    /** @return 当前可借数量。 */
    int availableCopies() const;

    /** @return 当前是否可借。 */
    bool isAvailable() const;

    /**
     * @brief 借出一本副本。
     * @return 借出成功返回 true，否则返回 false。
     */
    bool borrowCopy();

    /**
     * @brief 归还一本副本。
     * @return 归还成功返回 true，否则返回 false。
     */
    bool returnCopy();

    /** @return CSV 行。 */
    std::string toCsv() const;

    /**
     * @brief 从 CSV 行解析图书。
     * @return 成功时返回图书，格式错误时返回 std::nullopt。
     */
    static std::optional<Book> fromCsv(const std::string& line);

private:
    std::string isbn_;
    std::string title_;
    std::string author_;
    int totalCopies_;
    int availableCopies_;
};
