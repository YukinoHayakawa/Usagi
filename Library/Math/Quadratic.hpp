#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <tuple>
#include <type_traits>

namespace usagi
{
template <typename T>
struct QuadraticSolution
{
    T imaginary = 0;
    T real_low = 0;
    T real_high;
};

// https://www.programiz.com/cpp-programming/examples/quadratic-roots
template <typename T>
auto solve_quadratic(T a, T b, T c) requires std::is_floating_point_v<T>
{
    assert(a != 0);

    T discriminant = b * b - 4 * a * c;
    QuadraticSolution<T> solution;

    if(discriminant > 0)
    {
        T x0 = (-b - std::sqrt(discriminant)) / (2 * a);
        T x1 = (-b + std::sqrt(discriminant)) / (2 * a);
        std::tie(solution.real_low, solution.real_high) = std::minmax(x0, x1);
    }
    else
    {
        solution.real_high = solution.real_low = -b / (2 * a);
        if(discriminant < 0)
            solution.imaginary = std::sqrt(-discriminant) / (2 * a);
    }

    return solution;
}
}
