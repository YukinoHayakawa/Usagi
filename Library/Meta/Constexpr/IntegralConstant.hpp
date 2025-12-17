#pragma once

namespace usagi::meta
{
/*
template <typename T, T Val>
struct IntegralConstantWithType
{
    using type               = T;
    static constexpr T value = Val;
};
*/

template <auto Val>
struct IntegralConstant
{
    using type                  = decltype(Val);
    static constexpr type value = Val;
};

template <auto Val>
using Lifted = IntegralConstant<Val>;

/*
 * In C++ even if a function is `constexpr`/`consteval` and evaluated in such a
 * context, the arguments passed to the function would never be considered
 * constant expressions. Therefore, a technique is to wrap the value in a
 * construct like `IntegralConstant`, using an instantiation of it as the
 * parameter type, and use this `lift` function to make the value into template
 * parameter list by template argument deduction. This is very useful when
 * writing `constexpr`/`consteval` functions when you simply want to pass a
 * goddamn `constexpr` value to the function but the compiler simply won't
 * analyze where the value came from and yells at you that your argument is
 * not a constant expression.
 * This is especially useful when writing overloaded operators. Imagine that
 * without such a technique, you will have to `template operator()<arg>()`. That
 * would be absolutely nightmare. But with this, you can have something like
 * `lift<1>() << lift<2>()` which looks much cleaner.
 */
template <auto Val>
consteval auto lift()
{
    return Lifted<Val>();
}

namespace static_tests
{
consteval
{
    constexpr auto op = []<int I>(Lifted<I>) { return I; };
    static_assert(lift<3>().value == 3);
    static_assert(op(lift<123>()) == 123);
}
} // namespace static_tests
} // namespace usagi::meta
