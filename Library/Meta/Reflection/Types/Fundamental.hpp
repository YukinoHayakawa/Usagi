#pragma once

#include <cstddef>
#include <cstdint>

#include <Usagi/Library/Meta/Reflection/StaticReflection.hpp>

// todo: bug [libc++] can't reflect std::intN_t and alike
//   https://github.com/bloomberg/clang-p2996/issues/230
namespace usagi::meta::reflection
{
consteval auto & get_fundamental_types()
{
    using namespace std::meta;

    // Using fixed widths to ensure we are getting the correct ints
    // https://en.cppreference.com/w/cpp/language/types.html
    static constexpr std::initializer_list<std::meta::info> type_list {
        ^^void,
        ^^std::nullptr_t,
        // Standard integer types
        dealias(^^int16_t),
        dealias(^^uint16_t),
        dealias(^^int32_t),
        dealias(^^uint32_t),
        dealias(^^int64_t),
        dealias(^^uint64_t),
        // Boolean type
        ^^bool,
        // Character types
        ^^char,
        dealias(^^int8_t),
        dealias(^^uint8_t),
        ^^wchar_t,
        ^^char8_t,
        ^^char16_t,
        ^^char32_t,
        // Standard floating-point types
        ^^float,
        ^^double,
        ^^long double,
    };
    return type_list;
}

template <std::meta::info Refl>
    requires (std::meta::is_type(Refl))
consteval std::string_view get_fundamental_type_camel_case_name()
{
    using namespace std::meta;

    if constexpr(Refl == ^^void)
        return "Void";
    else if constexpr(Refl == ^^std::nullptr_t)
        return "Nullptr";
    // Standard integer types
    else if constexpr(Refl == dealias(^^int16_t))
        return "Integer16";
    else if constexpr(Refl == dealias(^^uint16_t))
        return "UnsignedInteger16";
    else if constexpr(Refl == dealias(^^int32_t))
        return "Integer32";
    else if constexpr(Refl == dealias(^^uint32_t))
        return "UnsignedInteger32";
    else if constexpr(Refl == dealias(^^int64_t))
        return "Integer64";
    else if constexpr(Refl == dealias(^^uint64_t))
        return "UnsignedInteger64";
    // Boolean type
    else if constexpr(Refl == ^^bool)
        return "Bool";
    // Character types
    else if constexpr(Refl == ^^char)
        return "Char";
    else if constexpr(Refl == dealias(^^int8_t))
        return "Integer8";
    else if constexpr(Refl == dealias(^^uint8_t))
        return "UnsignedInteger8";
    else if constexpr(Refl == ^^wchar_t)
        return "WideChar";
    else if constexpr(Refl == ^^char8_t)
        return "Char8";
    else if constexpr(Refl == ^^char16_t)
        return "Char16";
    else if constexpr(Refl == ^^char32_t)
        return "Char32";
    // Standard floating-point types
    else if constexpr(Refl == ^^float)
        return "Float";
    else if constexpr(Refl == ^^double)
        return "Double";
    else if constexpr(Refl == ^^long double)
        return "LongDouble";
    else
        static_assert(false, std::meta::display_string_of(Refl));
}

namespace static_tests
{
consteval
{
    static_assert(
        get_fundamental_type_camel_case_name<^^bool>() ==
        std::string_view("Bool")
    );
}
} // namespace static_tests
} // namespace usagi::meta::reflection
