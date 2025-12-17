#pragma once

#include <Usagi/Library/Meta/Constexpr/IntegralConstant.hpp>

namespace usagi::meta::reflection
{
/*
 * Lifts the reflection of a value.
 */
template <auto Val>
consteval auto meta_lift()
{
    return Lifted<std::meta::reflect_constant(Val)>();
}

namespace static_tests
{
consteval
{
    constexpr auto op = []<std::meta::info I>(Lifted<I>) { return [:I:]; };
    static_assert(meta_lift<3>().value == std::meta::reflect_constant(3));
    static_assert(op(meta_lift<123>()) == 123);
}
} // namespace static_tests
} // namespace usagi::meta::reflection
