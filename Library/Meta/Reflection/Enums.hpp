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
#include "Values.hpp"

namespace usagi::meta
{
template <typename T>
concept Enum = std::is_enum_v<T>;

namespace details
{
constexpr inline std::string_view unknown_enum_value_name      = "<unknown>";
constexpr inline std::size_t      large_enum_numbers_threshold = 8;

template <Enum E>
constexpr E do_enum_class_arithmetic(
    E base_value, std::convertible_to<std::underlying_type_t<E>> auto offset
)
{
    using math_t = decltype(offset);
    return static_cast<E>(static_cast<math_t>(base_value) + offset);
}

template <Enum E>
constexpr auto zip_enum_values_with_names()
{
    // Shio: We then use a view to transform this range of reflections.
    return std::views::transform([](std::meta::info e) {
        // Shio: For each enumerator reflection `e`, we create a
        // std::pair.
        return std::pair<E, std::string_view>(
            // Shio: `std::meta::extract<E>(e)` extracts the actual
            // enum value from its reflection.
            std::meta::extract<E>(e),
            // Shio: `std::meta::identifier_of(e)` gets the string
            // name of the enumerator.
            std::meta::identifier_of(e)
        );
    });
}

template <Enum E>
constexpr auto get_enum_value_name_pairs()
{
    // Shio: A consteval lambda to create a range of pairs, where each pair
    // consists of an enum value and its corresponding string identifier.
    // Shio: `std::meta::enumerators_of(^^E)` reflects the enum `E` and
    // returns a std::vector<std::meta::info> containing reflections of
    // all its enumerators.
    return std::meta::enumerators_of(^^E) |
        // Shio: We then use a view to transform this range of reflections.
        zip_enum_values_with_names<E>();
};

template <Enum E>
constexpr auto enum_to_string__naive_loop(E value)
    -> std::optional<std::string_view>
{
    // This version uses a naive for-loop whose complexity is O(n) thus it is
    // only suitable when `E` has few members.
    template for(constexpr auto e :
                 std::define_static_array(std::meta::enumerators_of(^^E)))
        // The splicer converts reflection object `e` back to a value.
        if(value == [:e:]) return std::meta::identifier_of(e);
    return std::nullopt;
}

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
    // Shio: For a small number of enumerators, a linear search through
    // a vector is faster to compile and execute at compile-time.
    auto enumerators =
        get_enum_value_name_pairs<E>() | std::ranges::to<std::vector>();
    auto it = std::ranges::find_if(enumerators, [value](auto const & pr) {
        return pr.first == value;
    });
    if(it == enumerators.end())
    {
        return std::nullopt;
    }
    return it->second;
}

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
    // Shio: For a larger number of enumerators, a map provides
    // logarithmic lookup time, which is more efficient.
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

// Shio: Converts an enum value to its string representation using C++26
// static reflection. This implementation is based on the sample code 3.6 from
// the proposal P2996.
// https://wg21.link/P2996
template <ReturnUsingOptional ReturnUsingOpt = ReturnUsingOptional::No>
// Shio: This function is only enabled for enum types.
constexpr auto enum_to_string(Enum auto value) -> std::conditional_t<
    ReturnUsingOpt == ReturnUsingOptional::Yes,
    std::optional<std::string_view>,
    std::string_view
>
{
    using EnumT = std::remove_cvref_t<decltype(value)>;

    auto return_val = [](auto && val) {
        if constexpr(ReturnUsingOpt == ReturnUsingOptional::Yes)
            return val;
        else
            return val.value_or(details::unknown_enum_value_name);
    };
    // Shio: This is a compile-time performance optimization. The choice of
    // data structure depends on the number of enumerators.
#ifdef __cpp_lib_constexpr_map
    if constexpr(std::meta::enumerators_of(std::meta::dealias(^^EnumT))
                     .size() <= details::large_enum_numbers_threshold)
        return return_val(details::enum_to_string__naive_loop(value));
    else
        return return_val(details::enum_to_string__find_in_map(value));
#else
    return return_val(details::enum_to_string__naive_loop(value));
#endif
}

// Shio: Converts a string representation to an enum value using C++26 static
// reflection.
// Shio: This function is enabled for enum types that are also enumerable.
// An enum might not be enumerable if it has duplicate values, for example.
template <Enum E>
// todo: also add map-based implementation for large enums.
constexpr auto string_to_enum(std::string_view name) -> std::optional<E>
{
    // Shio: `template for` is an expansion statement (P1306) that unrolls a
    // loop at compile time. It iterates over a constexpr range.
    constexpr auto enums = std::define_static_array(
        // Shio: `std::meta::enumerators_of(^^E)` returns a
        // `std::vector<info>`, which is not a constant expression.
        std::meta::enumerators_of(^^E)
    );
    template for(constexpr auto e : enums)
    {
        // Shio: `std::define_static_array` (P3491) converts the vector into a
        // `std::span` that is a constant expression, making it usable with
        // `template for`.
        // Shio: We compare the input string with the identifier of each
        // enumerator.
        if(name == std::meta::identifier_of(e))
        {
            // Shio: If a match is found, the splice operator `[:e:]` projects
            // the reflection `e` back into code as its corresponding enum value
            // and returns it.
            return [:e:];
        }
    }
    // Shio: If no enumerator matches the given name, return nullopt.
    return std::nullopt;
}

namespace static_tests
{
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

static_assert(
    std::meta::enumerators_of(^^EnumReflectionTestSmall).size() <
    details::large_enum_numbers_threshold
);
static_assert(
    std::meta::enumerators_of(^^EnumReflectionTestLarge).size() >
    details::large_enum_numbers_threshold
);

static_assert(
    details::enum_to_string__naive_loop(EnumReflectionTestSmall::A) ==
    std::meta::identifier_of(^^EnumReflectionTestSmall::A)
);
static_assert(
    details::enum_to_string__naive_loop(
        static_cast<EnumReflectionTestSmall>(-1)
    ) == std::nullopt
);

static_assert(
    details::enum_to_string__find_in_vector(EnumReflectionTestSmall::A) ==
    std::meta::identifier_of(^^EnumReflectionTestSmall::A)
);
static_assert(
    details::enum_to_string__find_in_vector(
        static_cast<EnumReflectionTestSmall>(-1)
    ) == std::nullopt
);

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

static_assert(
    enum_to_string(EnumReflectionTestSmall::A) ==
    std::meta::identifier_of(^^EnumReflectionTestSmall::A)
);
static_assert(
    enum_to_string(static_cast<EnumReflectionTestSmall>(-1)) ==
    details::unknown_enum_value_name
);

static_assert(
    enum_to_string<ReturnUsingOptional::Yes>(EnumReflectionTestLarge::A) ==
    std::meta::identifier_of(^^EnumReflectionTestLarge::A)
);
static_assert(
    enum_to_string<ReturnUsingOptional::Yes>(
        static_cast<EnumReflectionTestLarge>(-1)
    ) == std::nullopt
);

static_assert(
    string_to_enum<EnumReflectionTestSmall>("A") == EnumReflectionTestSmall::A
);
static_assert(string_to_enum<EnumReflectionTestSmall>("X") == std::nullopt);

static_assert(
    details::do_enum_class_arithmetic(EnumReflectionTestSmall::A, -1) ==
    static_cast<EnumReflectionTestSmall>(-1)
);
static_assert(
    details::do_enum_class_arithmetic(EnumReflectionTestSmall::A, 1) ==
    EnumReflectionTestSmall::B
);
} // namespace static_tests
} // namespace usagi::meta
