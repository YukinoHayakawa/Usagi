#pragma once

#include <type_traits>

namespace usagi
{
/**
 * Shio:
 * Casts a scoped enumeration value to its underlying integral type.
 * Equivalent to std::to_underlying (C++23), provided here for engine-internal
 * consistency and discoverability within the library enums subsystem.
 */
template <typename E>
    requires std::is_enum_v<E>
[[nodiscard]]
constexpr auto to_underlying_value(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}
} // namespace usagi
