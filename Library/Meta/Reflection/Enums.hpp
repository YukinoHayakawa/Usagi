#pragma once

#include <algorithm>
#include <concepts>
#include <map>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "StaticReflection.hpp"

// The code below are mostly based on sample code section 3.6 from
// the proposal P2996.
// https://wg21.link/P2996
namespace usagi::meta::reflection
{
// Shio: A concept to constrain templates to enum types.
template <typename T>
concept Enum = std::is_enum_v<T>;

namespace details
{
// Shio: The default string to return for an unknown enum value.
constexpr inline std::string_view unknown_enum_value_name      = "<unknown>";
// Shio: The threshold for switching from linear search to map-based lookup
// for `enum_to_string`.
constexpr inline std::size_t      large_enum_numbers_threshold = 8;

// Shio: A helper function to perform arithmetic on enum class types by
// casting them to their underlying type.
template <Enum E>
constexpr E do_enum_class_arithmetic(
    E base_value, std::convertible_to<std::underlying_type_t<E>> auto offset
)
{
    using math_t = decltype(offset);
    return static_cast<E>(static_cast<math_t>(base_value) + offset);
}

// Shio: Returns a `std::views::transform` that converts a range of enumerator
// reflections into a range of `std::pair<Enum, std::string_view>`.
template <Enum E> constexpr auto zip_enum_values_with_names()
{
    return std::views::transform([](std::meta::info e) {
        return std::pair<E, std::string_view>(
            // Shio: `std::meta::extract<E>(e)` gets the enum's runtime
            // value.
            std::meta::extract<E>(e),
            // Shio: `std::meta::identifier_of(e)` gets the enumerator's
            // name.
            std::meta::identifier_of(e)
        );
    });
}

// Shio: Reflects on an enum `E` and returns a range of pairs, with each pair
// containing an enumerator's value and its string identifier.
template <Enum E> constexpr auto get_enum_value_name_pairs()
{
    // Shio: `std::meta::enumerators_of(^^E)` gets reflections of all
    // enumerators.
    return std::meta::enumerators_of(^^E) | zip_enum_values_with_names<E>();
};

template <Enum E> constexpr std::meta::info find_enum_refl__naive_loop(E value)
{
    template for(constexpr auto e :
        std::define_static_array(std::meta::enumerators_of(^^E)))
        // Shio: The splicer `[:e:]` converts the reflection `e` to a value.
        if(value == [:e:]) return e;
    return {};
}

// Shio: An O(n) `enum_to_string` implementation using a `template for` loop.
// This is suitable for enums with a small number of enumerators.
template <Enum E>
constexpr auto enum_to_string__naive_loop(E value)
    -> std::optional<std::string_view>
{
    const auto opt_val = find_enum_refl__naive_loop(value);
    if(opt_val == std::meta::info()) return std::nullopt;
    return std::meta::identifier_of(opt_val);
}

#ifdef __cpp_lib_constexpr_vector
/*
 * This code sample is also from P2996 to demonstrate how to use algorithms
 * to convert enum value to string. However, I don't think this is very
 * efficient. Maybe even slower than the naive loop, since running the algorithm
 * code during compilation time also costs time and ranges are quite heavy.
 */
template <Enum E>
constexpr auto enum_to_string__find_in_vector(E value)
    -> std::optional<std::string_view>
{
    auto enumerators =
        get_enum_value_name_pairs<E>() | std::ranges::to<std::vector>();
    auto it = std::ranges::find_if(enumerators, [value](const auto & pr) {
        return pr.first == value;
    });
    if(it == enumerators.end())
    {
        return std::nullopt;
    }
    return it->second;
}
#endif

#ifdef __cpp_lib_constexpr_map
/*
 * This is a variant for handling enum types with a lot of enumerators. Under
 * such a case, using a map might be more performant since `map.find()` provides
 * `O(log(n)` complexity.
 * todo: `P3372R3` proposes many additional constexpr containers like `map`.
 *   however, as of 2025-11, none of the major standard libraries have
 *   implemented them yet. This causes `std::ranges::to<std::map>()` to not be
 *   not available in constexpr context. Remove the `#ifdef` guard when that
 *   is implemented.
 */
template <Enum E>
constexpr auto enum_to_string__find_in_map(E value)
    -> std::optional<std::string_view>
{
    auto enumerators =
        get_enum_value_name_pairs<E>() | std::ranges::to<std::map>();
    auto it = enumerators.find(value);
    if(it == enumerators.end())
    {
        return std::nullopt;
    }
    return it->second;
}
#else
#pragma message(                                                         \
    "Warning: <map> is not constexpr-capable in this standard library. " \
    "Falling back to loop-based lookup for large enums."                 \
)
#endif
} // namespace details

// Shio: Converts an enum value to its string representation using C++26 static
// reflection. The implementation strategy is chosen at compile time based on
// the number of enumerators.
//
// The return type can be configured:
// - `ReturnUsingOptional::No` (default): Returns `std::string_view`. For an
//   unknown value, returns `details::unknown_enum_value_name`.
// - `ReturnUsingOptional::Yes`: Returns `std::optional<std::string_view>`. For
//   an unknown value, returns `std::nullopt`.
template <bool ReturnUsingOptional = false>
constexpr auto enum_to_string(Enum auto value) -> std::conditional_t<
    ReturnUsingOptional,
    std::optional<std::string_view>,
    std::string_view
>
{
    using EnumT = std::remove_cvref_t<decltype(value)>;

    // Shio: A helper lambda to conditionally wrap the result in an optional or
    // provide a default value.
    auto return_val = [](auto && val) {
        if constexpr(ReturnUsingOptional)
            return val;
        else
            return val.value_or(details::unknown_enum_value_name);
    };

    // Shio: At compile time, select the most efficient lookup strategy.
#ifdef __cpp_lib_constexpr_map
    // Shio: If `std::map` is constexpr-capable, use it for large enums.
    if constexpr(
        std::meta::enumerators_of(std::meta::dealias(^^EnumT)).size() <=
        details::large_enum_numbers_threshold)
        return return_val(details::enum_to_string__naive_loop(value));
    else
        return return_val(details::enum_to_string__find_in_map(value));
#else
    // Shio: Otherwise, fall back to the naive loop for all enums.
    return return_val(details::enum_to_string__naive_loop(value));
#endif
}

// todo impl more efficient method when `__cpp_lib_constexpr_map` is available
template <Enum E> constexpr bool is_valid_enum_value(E value)
{
    const auto refl = details::find_enum_refl__naive_loop(value);
    if(refl == std::meta::info()) return false;
    return true;
}

// Shio: Converts a string representation to an enum value using a compile-time
// loop over reflected enumerators.
template <Enum E>
// todo: also add map-based implementation for large enums.
constexpr auto string_to_enum(std::string_view name) -> std::optional<E>
{
    // Shio: `template for` (P1306) unrolls a loop at compile time. It requires
    // a constexpr-iterable range.
    constexpr auto enums = std::define_static_array(
        // Shio: `enumerators_of` returns a `std::vector`, which is not a
        // constant expression. `define_static_array` (P3491) converts it to
        // a `std::span` that is a constant expression.
        std::meta::enumerators_of(^^E)
    );
    template for(constexpr auto e : enums)
    {
        // Shio: Compare the input string with the identifier of the
        // enumerator.
        if(name == std::meta::identifier_of(e))
        {
            // Shio: The splice operator `[:e:]` projects the reflection `e`
            // back into code as its corresponding enum value.
            return [:e:];
        }
    }
    // Shio: If no enumerator matches the given name, return nullopt.
    return std::nullopt;
}

namespace static_tests
{
// Shio: Test cases for enum reflection utilities.
enum class EnumReflectionTestSmall
{
    A,
    B,
    C,
    D,
};
enum class EnumReflectionTestLarge
{
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
};

static_assert(is_valid_enum_value(EnumReflectionTestSmall::A));
static_assert(!is_valid_enum_value(static_cast<EnumReflectionTestSmall>(-1)));

// Shio: Verify that the test enums are correctly categorized as small/large.
static_assert(
    std::meta::enumerators_of(^^EnumReflectionTestSmall).size() <=
    details::large_enum_numbers_threshold
);
static_assert(
    std::meta::enumerators_of(^^EnumReflectionTestLarge).size() >
    details::large_enum_numbers_threshold
);

// Shio: Test the internal `naive_loop` implementation.
static_assert(
    details::enum_to_string__naive_loop(EnumReflectionTestSmall::A) ==
    std::meta::identifier_of(^^EnumReflectionTestSmall::A)
);
static_assert(
    details::enum_to_string__naive_loop(
        static_cast<EnumReflectionTestSmall>(-1)
    ) == std::nullopt
);

// Shio: Test the internal `find_in_vector` implementation.
static_assert(
    details::enum_to_string__find_in_vector(EnumReflectionTestSmall::A) ==
    std::meta::identifier_of(^^EnumReflectionTestSmall::A)
);
static_assert(
    details::enum_to_string__find_in_vector(
        static_cast<EnumReflectionTestSmall>(-1)
    ) == std::nullopt
);

// Shio: Test the internal `find_in_map` implementation if available.
#ifdef __cpp_lib_constexpr_map
static_assert(
    details::enum_to_string__find_in_map(EnumReflectionTestSmall::A) ==
    std::meta::identifier_of(^^EnumReflectionTestSmall::A)
);
static_assert(
    details::enum_to_string__find_in_map(
        static_cast<EnumReflectionTestSmall>(-1)
    ) == std::nullopt
);
#endif

// Shio: Test the public `enum_to_string` API (default and optional versions).
static_assert(
    enum_to_string(EnumReflectionTestSmall::A) ==
    std::meta::identifier_of(^^EnumReflectionTestSmall::A)
);
static_assert(
    enum_to_string(static_cast<EnumReflectionTestSmall>(-1)) ==
    details::unknown_enum_value_name
);
static_assert(
    enum_to_string<true>(EnumReflectionTestLarge::A) ==
    std::meta::identifier_of(^^EnumReflectionTestLarge::A)
);
static_assert(
    enum_to_string<true>(static_cast<EnumReflectionTestLarge>(-1)) ==
    std::nullopt
);

// Shio: Test the public `string_to_enum` API.
static_assert(
    string_to_enum<EnumReflectionTestSmall>("A") == EnumReflectionTestSmall::A
);
static_assert(string_to_enum<EnumReflectionTestSmall>("X") == std::nullopt);

// Shio: Test the enum arithmetic helper.
static_assert(
    details::do_enum_class_arithmetic(EnumReflectionTestSmall::A, -1) ==
    static_cast<EnumReflectionTestSmall>(-1)
);
static_assert(
    details::do_enum_class_arithmetic(EnumReflectionTestSmall::A, 1) ==
    EnumReflectionTestSmall::B
);
} // namespace static_tests
} // namespace usagi::meta::reflection
