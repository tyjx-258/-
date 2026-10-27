#pragma once

#include "Book.h"
#include "BorrowRecord.h"
#include "Reader.h"

#include <string>
#include <vector>

/**
 * @brief 图书借阅系统聚合根，组合管理图书、读者和借阅记录。
 */
class Library {
public:
    /** @param dataDir CSV 数据文件所在目录。 */
    explicit Library(std::string dataDir);

    /**
     * @brief 从数据目录加载全部记录。
     * @param message 返回加载结果说明。
     */
    bool load(std::string& message);

    /**
     * @brief 保存全部记录到数据目录。
     * @param message 返回保存结果说明。
     */
    bool save(std::string& message) const;

    /** @brief 登记一本新书。 */
    bool addBook(const std::string& isbn,
                 const std::string& title,
                 const std::string& author,
                 int totalCopies,
                 std::string& message);

    /** @brief 登记一位新读者。 */
    bool addReader(const std::string& readerId,
                   const std::string& name,
                   const std::string& readerType,
                   std::string& message);

    /** @brief 办理借书。 */
    bool borrowBook(const std::string& readerId,
                    const std::string& isbn,
                    std::string& message);

    /** @brief 办理还书。 */
    bool returnBook(const std::string& readerId,
                    const std::string& isbn,
                    std::string& message);

    /** @return 按 ISBN 或书名关键字匹配的图书副本。 */
    std::vector<Book> searchBooks(const std::string& keyword) const;

    /** @return 全部图书。 */
    const std::vector<Book>& books() const;

    /** @return 全部读者。 */
    const std::vector<Reader>& readers() const;

    /** @return 全部借阅记录。 */
    const std::vector<BorrowRecord>& records() const;

private:
    bool loadBooks(std::string& message);
    bool loadReaders(std::string& message);
    bool loadRecords(std::string& message);

    Book* findBookMutable(const std::string& isbn);
    Reader* findReaderMutable(const std::string& readerId);
    const Book* findBook(const std::string& isbn) const;
    const Reader* findReader(const std::string& readerId) const;

    std::size_t activeBorrowCount(const std::string& readerId) const;
    int nextRecordId() const;
    std::string joinPath(const std::string& fileName) const;

    std::string dataDir_;
    std::vector<Book> books_;
    std::vector<Reader> readers_;
    std::vector<BorrowRecord> records_;
};
