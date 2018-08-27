#pragma once

// Requires _SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING
// Requires _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>

namespace usagi
{
// Constants

template <typename T> constexpr T M_E           = T(2.71828182845904523536);       // e
template <typename T> constexpr T M_LOG2E       = T(1.44269504088896340736);       // log2(e)
template <typename T> constexpr T M_LOG10E      = T(0.434294481903251827651);      // log10(e)
template <typename T> constexpr T M_LN2         = T(0.693147180559945309417);      // ln(2)
template <typename T> constexpr T M_LN10        = T(2.30258509299404568402);       // ln(10)
template <typename T> constexpr T M_PI          = T(3.14159265358979323846);       // pi
template <typename T> constexpr T M_PI_2        = T(1.57079632679489661923);       // pi/2
template <typename T> constexpr T M_PI_4        = T(0.785398163397448309616);      // pi/4
template <typename T> constexpr T M_1_PI        = T(0.318309886183790671538);      // 1/pi
template <typename T> constexpr T M_2_PI        = T(0.636619772367581343076);      // 2/pi
template <typename T> constexpr T M_2_SQRTPI    = T(1.12837916709551257390);       // 2/sqrt(pi)
template <typename T> constexpr T M_SQRT2       = T(1.41421356237309504880);       // sqrt(2)
template <typename T> constexpr T M_SQRT1_2     = T(0.707106781186547524401);      // 1/sqrt(2)
template <typename T> constexpr T M_180_DEGS    = T(180);                          // 180 degrees

// Vectors

using Vector2f = Eigen::Vector2f;
using Vector2i = Eigen::Vector2i;
using Vector2u32 = Eigen::Matrix<std::uint32_t, 2, 1>;
using Vector2i32 = Eigen::Matrix<std::int32_t, 2, 1>;
using Vector3f = Eigen::Vector3f;
using Vector4f = Eigen::Vector4f;

// Color

using Color4f = Vector4f;

// Angles

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

// Transformation

using Matrix3f = Eigen::Matrix3f;
using Matrix4f = Eigen::Matrix4f;
using Affine3f = Eigen::Affine3f;
using Projective3f = Eigen::Projective3f;
}
