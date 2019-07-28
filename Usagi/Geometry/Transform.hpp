#pragma once

#include <Usagi/Math/Matrix.hpp>

namespace usagi
{
template <typename VectorT>
struct Point
{
    const VectorT &value;

    Point(const VectorT &value)
        : value(value)
    {
    }
};

template <typename VectorT>
struct Vector
{
    const VectorT &value;

    Vector(const VectorT &value)
        : value(value)
    {
    }
};

template <typename VectorT>
struct Normal
{
    const VectorT &value;

    Normal(const VectorT &value)
        : value(value)
    {
    }
};

template <typename Scalar, int Dim, int Mode, typename Vec>
auto operator*(const Eigen::Transform<Scalar, Dim, Mode, 0> &transform,
    const Vector<Vec> &vector) -> Eigen::Matrix<Scalar, Dim, 1>
{
    Eigen::Matrix<Scalar, Dim + 1, 1> affine_vec;
    affine_vec.template head<Dim>() = vector.value;
    affine_vec(Dim) = 0;
    return (transform * affine_vec).template head<Dim>();
}

template <typename Scalar, int Dim, int Mode, typename Vec>
auto operator*(const Eigen::Transform<Scalar, Dim, Mode, 0> &transform,
    const Point<Vec> &point) -> Eigen::Matrix<Scalar, Dim, 1>
{
    return (transform * point.value.homogeneous()).template head<Dim>();
}
}
