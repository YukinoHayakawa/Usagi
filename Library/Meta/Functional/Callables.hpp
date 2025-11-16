#pragma once

#include <tuple>
#include <utility>

#include <Usagi/Library/Meta/Reflection/StaticReflection.hpp>

namespace usagi::meta::functional
{
// Create a callable lambda from a free template function
template <std::meta::info FreeFuncRefl>
constexpr auto make_callable()
{
    // constexpr static auto refl = ^^Func;
    return []<typename... Args>(Args &&... args) {
        static_assert(std::meta::can_substitute(FreeFuncRefl, { ^^Args... }));
        // I have tried many ways to check whether the resulted object is
        // actually callable. However, it seems that I just cannot get it to
        // work. Anyway, the static_assert above is actually enough.

        // constexpr auto func =
        //     std::meta::substitute(FreeFuncRefl, std::vector { ^^Args... });
        // static_assert(std::meta::is_invocable_type(
        //     func, std::vector { ^^Args... }));
        // static_assert(std::is_invocable_v(
        //     [:func:], std::vector { ^^Args... }));

        // a reflection of a function template cannot represent a template
        // argument static_assert([:std::meta::substitute(^^std::is_invocable_v,
        //    std::vector { func, ^^Args... }):]);
        // static_assert(std::is_invocable_v<
        //     std::meta::extract<void(Args&&...)>(func), Args...>);
        // static_assert(std::is_invocable_v<
        //     std::meta::extract<
        //     std::meta::reflect_function([:func:](args...))>(func), Args...>);

        // a reflection of a function template cannot represent a template
        // argument static_assert([:std::meta::substitute(
        //     ^^std::is_invocable_v,
        //     std::define_static_array(std::vector { FreeFuncRefl, ^^Args... })
        // ):]);

        // ... So I gave up.

        return [:std::meta::substitute(FreeFuncRefl, { ^^Args... }):](
            std::forward<Args>(args)...
        );
    };
}

namespace static_tests
{
template <typename T>
constexpr auto && identity(T && t)
{
    return std::forward<T>(t);
}

template <typename T, typename V>
constexpr auto it_takes_two(T && t, V && v)
{
    return std::make_pair(t, v);
}

static_assert(make_callable<^^identity>()(1) == 1);
static_assert(
    make_callable<^^it_takes_two>()(1, true) == std::make_pair(1, true)
);
} // namespace static_tests
} // namespace usagi::meta::functional
