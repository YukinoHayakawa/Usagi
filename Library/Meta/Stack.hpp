#pragma once

#include <array>

#include <Usagi/Concept/Container/Stack.hpp>

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

    constexpr void pop()
    {
        --pos;
    }

    constexpr const int & top()
    {
        return stack[pos - 1];
    }

    constexpr bool empty() const
    {
        return pos == 0;
    }

    constexpr std::size_t size() const
    {
        return pos;
    }
};
}

static_assert(usagi::concepts::Stack<usagi::meta::Stack<1>, int>);
