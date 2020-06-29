#pragma once

#include <array>

namespace usagi::meta
{
template <int Size>
struct Stack
{
    std::array<int, Size> stack { };
    std::size_t pos = 0;

    constexpr void push(const int value)
    {
        stack[pos++] = value;
    }

    constexpr int pop()
    {
        return stack[--pos];
    }

    constexpr bool empty() const
    {
        return pos == 0;
    }
};
}
