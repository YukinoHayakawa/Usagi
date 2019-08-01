#pragma once

// Requires _SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING
// Requires _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>

namespace usagi
{
// Vectors

template <typename T, int Size>
using Vector = Eigen::Matrix<T, Size, 1>;
using Vector2f = Eigen::Vector2f;
using Vector2d = Eigen::Vector2d;
using Vector2i = Eigen::Vector2i;
using Vector2u32 = Vector<std::uint32_t, 2>;
using Vector2i32 = Vector<std::int32_t, 2>;
using Vector3f = Eigen::Vector3f;
using Vector4f = Eigen::Vector4f;

// Transformation

using Quaternionf = Eigen::Quaternionf;

using Matrix3f = Eigen::Matrix3f;
using Matrix4f = Eigen::Matrix4f;
using Affine3f = Eigen::Affine3f;
using Projective3f = Eigen::Projective3f;

// don't know why but MSVC forces me to spell out all arguments even those with
// default parameters, maybe a bug.
// https://developercommunity.visualstudio.com/content/problem/449437/msvc-cannot-deduce-template-argument-from-a-class.html
// template <int OtherDim, typename Scalar, int Dim>
// auto resize(const Eigen::Matrix<Scalar, Dim, 1> &vec)
template <int OtherDim, typename Scalar, int Dim, auto... Args>
auto resize(const Eigen::Matrix<Scalar, Dim, 1, Args...> &vec)
{
    Eigen::Matrix<Scalar, OtherDim, 1> r;
    // extend size
    if constexpr(OtherDim > Dim)
    {
        r = decltype(r)::Zero();
        r.template head<Dim>() = vec;
    }
    // shrink size or unchanged
    else
    {
        r = vec.template head<OtherDim>();
    }
    return r;
}
}
