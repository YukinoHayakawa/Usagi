#pragma once

#include <type_traits>

#include <Usagi/Library/Meta/Constexpr/IntegralConstant.hpp>
#include <Usagi/Library/Meta/Reflection/StaticReflection.hpp>

namespace usagi::meta::functional
{
/*
 * Only when we use a reflection of a template can we support non-type template
 * parameters.
 */
template <std::meta::info TemplateRefl, std::meta::info... PartialArgs>
    requires (std::meta::is_template(TemplateRefl))
struct template_partial_apply
{
    template <std::meta::info... RemainingArgs>
    consteval static auto apply(/*Lifted<RemainingArgs>...*/)
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
            std::array<
                std::meta::info,
                sizeof...(PartialArgs) + sizeof...(RemainingArgs)
            > {
                PartialArgs...,
                RemainingArgs...,
            }
        );
        constexpr bool is_complete_v =
            std::meta::can_substitute(TemplateRefl, applied_args);
        // If the template is fully specialized, return it.
        if constexpr(is_complete_v)
        {
            return std::meta::substitute(TemplateRefl, applied_args);
        }
        // Otherwise, return a new partially applied template.
        else
        {
            return template_partial_apply<
                TemplateRefl,
                PartialArgs...,
                RemainingArgs...
            >();
        }
    }
};

template <
    std::meta::info TemplateRefl,
    std::meta::info NextArg,
    std::meta::info... PartialArgs
>
consteval auto operator<<(
    template_partial_apply<TemplateRefl, PartialArgs...> p,
    // NextArg must be lifted to a constant expression by making it into a
    // template argument.
    Lifted<NextArg>
)
{
    return decltype(p)::template apply<NextArg>();
}

// This function must be able to handle the case where no `PartialArgs` are
// provided.
template <std::meta::info TemplateRefl, std::meta::info... PartialArgs>
    requires (std::meta::is_template(TemplateRefl))
// Unfortunately we can only have one `Lifted` parameter here, because if we
// have multiple of them, more than one implicit conversions are required and
// this function won't be viable.
consteval auto partial_apply(/*Lifted<TemplateRefl>*/)
{
    using partial_t = template_partial_apply<TemplateRefl, PartialArgs...>;
    // If the template is already fully specialized, return it.
    if constexpr(!std::is_same_v<decltype(partial_t::apply()), partial_t>)
    {
        return partial_t::apply();
    }
    else
    {
        return partial_t { };
    }
}

/*
consteval auto partial_apply(
    std::meta::info template_refl, std::meta::info... partial_args
)
{
    // can't lift arguments because they are not constant expressions
    constexpr auto lifter = []<std::meta::info I>(Lifted<I> l) { return l; };
    return partial_apply(
        lifter(std::meta::reflect_constant(template_refl)),
        lifter(partial_args)...
    );
}
*/

namespace static_tests
{
struct __currying_test_type
{
    int c = 31'415;
};

template <typename>
struct __currying_test_template_arg
{
};

template <typename A, int B, template <typename> typename C>
struct __currying_test_host
{
    constexpr static A   a { };
    constexpr static int b = B;
};

consteval
{
    // clang-format off

    // Applying no template arguments.
    static_assert(std::is_same_v<
        decltype(partial_apply<^^__currying_test_host>()),
        template_partial_apply<^^__currying_test_host>
    >);

    static_assert(std::is_same_v<
        decltype(partial_apply<^^__currying_test_host, ^^int>()),
        decltype(template_partial_apply<(^^__currying_test_host)>()
            << lift<^^int>())
    >);

    constexpr auto rc = std::meta::reflect_constant(1'024);

    // Applying one template argument.
    static_assert(
        partial_apply<^^__currying_test_host, ^^__currying_test_type>()
            .apply<rc, ^^__currying_test_template_arg>() ==
        ^^__currying_test_host<
            __currying_test_type,
            1'024,
            __currying_test_template_arg
        >
    );

    // Fully specializing the template in two passes and instantiate it.
    static_assert(typename[:
        partial_apply<^^__currying_test_host, ^^__currying_test_type, rc>()
            .apply<^^__currying_test_template_arg>()
        :]().a.c == 31'415
    );

    // Fully specializing the template in one pass and instantiate it.
    static_assert(typename[:
        partial_apply<
            ^^__currying_test_host,
            ^^__currying_test_type,
            rc,
            ^^__currying_test_template_arg
        >():]().b == 1'024
    );
    // clang-format on
}
} // namespace static_tests
} // namespace usagi::meta::functional
