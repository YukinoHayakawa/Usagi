#pragma once

#include <thread>
#include <typeindex>

#include <Usagi/Concepts/Type/Types.hpp>

namespace usagi
{
template <Arithmetic T>
std::string_view to_string_view(T &&val)
{
    return {
        reinterpret_cast<const char *>(&val),
        sizeof(T)
    };
}

template <StringView T>
std::string_view to_string_view(T &&val)
{
    return std::string_view(val);
}

template <Enum T>
std::string_view to_string_view(T &&val)
{
    using UnderlyingT = std::underlying_type_t<std::remove_cvref_t<T>>;
    return to_string_view(static_cast<UnderlyingT>(val));
}

inline std::string_view to_string_view(const std::thread::id val)
{
    return {
        reinterpret_cast<const char *>(&val),
        sizeof(std::thread::id)
    };
}

inline std::string_view to_string_view(const std::type_index val)
{
    return to_string_view(val.hash_code());
}
}
