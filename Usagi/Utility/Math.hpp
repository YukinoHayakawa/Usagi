#pragma once

#include <cmath>
#include <utility>

namespace usagi
{
template <typename T>
struct Interval
{
    T min, max;

    constexpr bool openContains(T value) const
    {
        return value > min && value < max;
    }
};

template <typename T>
constexpr auto solveQuadratic(T a, T b, T c)
{
    struct Solution
    {
        T discriminant;
        T t0 = 0, t1 = 0;
    } s;
    s.discriminant = b * b - 4 * a * c;
    if(s.discriminant >= 0)
    {
        const auto sd = std::sqrt(s.discriminant);
        const auto aa = a + a;
        s.t0 = (-b + sd) / aa;
        s.t1 = (-b - sd) / aa;
        // always make t0 the smaller root
        if(s.t0 > s.t1) std::swap(s.t0, s.t1);
    }
    return s;
}
}
