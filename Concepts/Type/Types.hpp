#pragma once

#include <string_view>
#include <type_traits>

namespace usagi
{
template <typename T>
concept Arithmetic = std::is_arithmetic_v<std::remove_cvref_t<T>>;

template <typename T>
concept StringView = std::is_constructible_v<std::string_view, const T &>;

template <typename T>
concept Enum = std::is_enum_v<std::remove_cvref_t<T>>;
}
