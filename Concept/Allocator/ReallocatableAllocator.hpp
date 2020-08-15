#pragma once

#include <concepts>

namespace usagi
{
template <typename T>
concept ReallocatableAllocator = requires (T t)
{
    { t.allocate(std::size_t()) } -> std::convertible_to<void *>;
    { t.reallocate(nullptr, std::size_t()) } -> std::convertible_to<void *>;
    { t.deallocate(nullptr) };
    { t.max_size() } -> std::same_as<std::size_t>;
};
}
