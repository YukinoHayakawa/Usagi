#pragma once

namespace usagi::meta
{
template <typename... Ts>
struct TypeList
{
    friend constexpr bool operator==(const TypeList &lhs, const TypeList &rhs)
    {
        return true;
    }

    friend constexpr bool operator!=(const TypeList &lhs, const TypeList &rhs)
    {
        return !(lhs == rhs);
    }
};
}
