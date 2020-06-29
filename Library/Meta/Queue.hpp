#pragma once

#include <array>

namespace usagi::meta
{
template <int Size>
struct Queue
{
    std::array<int, Size> queue { };
    std::size_t begin = 0, end = 0;

    constexpr void push(const int value)
    {
        queue[end++] = value;
    }

    constexpr int pop()
    {
        return queue[begin++];
    }

    constexpr bool empty() const
    {
        return begin == end;
    }

    constexpr int front() const
    {
        return queue[begin];
    }

    constexpr int back() const
    {
        return queue[end - 1];
    }
};
}
