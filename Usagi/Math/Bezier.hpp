#pragma once

#include <cmath>

#include <Usagi/Math/Matrix.hpp>

namespace usagi
{
template <typename T>
Vector<T, 2> cubicBezier(
    const Vector<T, 2> &c0,
    const Vector<T, 2> &c1,
    const Vector<T, 2> &c2,
    const Vector<T, 2> &c3,
    T t)
{
    // https://www.geeksforgeeks.org/cubic-bezier-curve-implementation-in-c/
    T xu = 0, yu = 0;
    xu = std::pow(1 - t, 3) * c0.x()
        + 3 * t * std::pow(1 - t, 2) * c1.x()
        + 3 * std::pow(t, 2) * (1 - t) * c2.x()
        + std::pow(t, 3) * c3.x();
    yu = std::pow(1 - t, 3) * c0.y()
        + 3 * t * std::pow(1 - t, 2) * c1.y()
        + 3 * std::pow(t, 2) * (1 - t) * c2.y()
        + std::pow(t, 3) * c3.y();
    return { xu, yu };
}

template <typename T>
T cssCubicBezier(T x1, T y1, T x2, T y2, T t)
{
    return cubicBezier<T>(
        { 0, 0 },
        { x1, y1 },
        { x2, y2 },
        { 1, 1 },
        t
    ).y();
}
}
