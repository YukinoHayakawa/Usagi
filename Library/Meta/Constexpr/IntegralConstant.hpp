#pragma once

namespace usagi::meta
{
template <typename T, T Val>
struct IntegralConstant
{
    using type               = T;
    static constexpr T value = Val;
};

template <auto Val>
consteval auto lift()
{
    return IntegralConstant<decltype(Val), Val>();
}

namespace static_tests
{
consteval
{
    constexpr auto op = []<int I>(IntegralConstant<int, I>) { return I; };
    static_assert(lift<3>().value == 3);
    static_assert(op(lift<123>()) == 123);
}
} // namespace static_tests
} // namespace usagi::meta
