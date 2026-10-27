#pragma once

#include "BorrowRule.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <string>

/**
 * @brief 读者实体，通过组合持有一份具体借阅规则。
 */
class Reader {
public:
    Reader(std::string id, std::string name, std::unique_ptr<BorrowRule> rule);

    Reader(Reader&&) noexcept = default;
    Reader& operator=(Reader&&) noexcept = default;
    Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;

    /** @return 读者编号。 */
    const std::string& id() const;

    /** @return 读者姓名。 */
    const std::string& name() const;

    /** @return 只读借阅规则。 */
    const BorrowRule& rule() const;

    /**
     * @brief 判断当前借阅数量是否低于规则上限。
     * @param currentCount 当前未归还数量。
     */
    bool canBorrow(std::size_t currentCount) const;

    /** @return CSV 行。 */
    std::string toCsv() const;

    /**
     * @brief 从 CSV 行解析读者。
     * @return 成功时返回读者，格式错误时返回 std::nullopt。
     */
    static std::optional<Reader> fromCsv(const std::string& line);

private:
    std::string id_;
    std::string name_;
    std::unique_ptr<BorrowRule> rule_;
};
