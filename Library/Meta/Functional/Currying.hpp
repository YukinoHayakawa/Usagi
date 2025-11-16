#pragma once

#include <type_traits>

#include <Usagi/Library/Meta/Reflection/StaticReflection.hpp>

namespace usagi::meta::functional
{
// unused flag type
struct incompletely_applied_template_t
{
};

template <template <typename...> typename Template, typename... PartialArgs>
struct partially_applied_template
{
    template <typename... RemainingArgs>
    consteval static auto apply()
    {
        // For some reason, I cannot write a vector like this here. Clang will
        // complain about "heap allocation".
        // std::vector applied_args { ^^PartialArgs..., ^^RemainingArgs... };

        // clang-format off
        // However, wrapping it inside a consteval lambda works.
        // constexpr auto applied_args = [] consteval {
        //     // Must specify `std::meta::info` here in case `PartialArgs` or
        //     // `RemainingArgs` is empty.
        //     return std::vector<std::meta::info> {
        //         ^^PartialArgs..., ^^RemainingArgs... };
        // };
        // clang-format on

        // `define_static_array` is the right way to go.
        constexpr auto applied_args = std::define_static_array(
            std::vector<std::meta::info> { ^^PartialArgs...,
                                           ^^RemainingArgs... }
        );
        constexpr bool is_complete_v =
            std::meta::can_substitute(^^Template, applied_args);
        // If the template is fully specialized, return it.
        if constexpr(is_complete_v)
        {
            return std::meta::substitute(^^Template, applied_args);
        }
        // Otherwise, return a new partially applied template.
        else
        {
            return partially_applied_template<
                Template, PartialArgs..., RemainingArgs...
            >();
        }
    }
};

// This function must be able to handle the case where no `PartialArgs` are
// provided.
template <template <typename...> typename Template, typename... PartialArgs>
consteval auto partially_apply()
{
    using partial_t = partially_applied_template<Template, PartialArgs...>;
    // If the template is already fully specialized, return it.
    if constexpr(!std::is_same_v<decltype(partial_t::apply()), partial_t>)
    {
        return ^^Template<PartialArgs...>;
    }
    else
    {
        return partial_t {};
    }
}

namespace static_tests
{
struct TestValue
{
    int c = 31'415;
};

template <typename A, typename B>
struct CurryingTest
{
    constexpr static A a {};
    constexpr static B b {};
};

// clang-format off
// Applying no template arguments.
static_assert(std::is_same_v<
    decltype(partially_apply<CurryingTest>()),
    partially_applied_template<CurryingTest>
>);

// Applying one template argument.
static_assert(
    partially_apply<CurryingTest, TestValue>().apply<char>() ==
    ^^CurryingTest<TestValue, char>
);

// Fully specializing the template in two passes and instantiate it.
static_assert(
    typename[:partially_apply<CurryingTest, TestValue>().apply<char>():]().a.c
    == 31'415);

// Fully specializing the template in one pass and instantiate it.
static_assert(
    typename[:partially_apply<CurryingTest, TestValue, char>():]().a.c
    == 31'415
);
// clang-format on
} // namespace static_tests
} // namespace usagi::meta::functional
