#pragma once

#include <eigen3/Eigen/Geometry>

#include "Constant.hpp"

namespace usagi
{
using AngleAxisf = Eigen::AngleAxisf;

template <typename T>
T degreesToRadians(const T degrees)
{
    return degrees * M_PI<T> / M_180_DEGS<T>;
}

template <typename T>
T radiansToDegrees(const T radians)
{
    return radians * M_180_DEGS<T> / M_PI<T>;
}
}
