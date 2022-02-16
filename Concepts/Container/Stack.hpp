#pragma once

#include <cstddef>
#include <concepts>

namespace usagi::concepts
{
template <typename T, typename R>
concept Stack = requires(T t, R r)
{
    { t.push(r) };
    { t.top() } -> std::convertible_to<const R &>;
    { t.pop() };
    { t.size() } -> std::same_as<std::size_t>;
    { t.empty() } -> std::same_as<bool>;
};
}
