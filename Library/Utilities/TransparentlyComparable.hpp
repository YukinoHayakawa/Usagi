#pragma once

#include <compare>

namespace usagi
{
template <typename Primary, typename Key>
struct TransparentlyComparable
{
    friend std::strong_ordering operator<=>(
        const Primary &lhs,
        const Primary &rhs)
    {
        return lhs.key() <=> rhs.key();
    }

    friend std::strong_ordering operator<=>(const Primary &lhs, const Key &rhs)
    {
        return lhs.key() <=> rhs;
    }

    friend std::strong_ordering operator<=>(const Key &lhs, const Primary &rhs)
    {
        return lhs <=> rhs.key();
    }
};
}
