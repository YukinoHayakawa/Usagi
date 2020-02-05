#pragma once

#include <cstddef>

namespace usagi
{
template <typename T>
concept ReallocatableAllocator = requires (T t)
{
    { t.allocate(std::size_t()) } -> void *;
    { t.reallocate(nullptr, std::size_t()) } -> void *;
    { t.deallocate(nullptr) } -> void;
    { t.max_size() } -> std::size_t;
};
}
