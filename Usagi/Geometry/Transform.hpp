#pragma once

#include <Usagi/Math/Matrix.hpp>

namespace usagi
{
template <typename VectorT>
struct AsPoint
{
    const VectorT &value;

    AsPoint(const VectorT &value)
        : value(value)
    {
    }
};

template <typename VectorT>
struct AsVector
{
    const VectorT &value;

    AsVector(const VectorT &value)
        : value(value)
    {
    }
};

template <typename VectorT>
struct AsNormal
{
    const VectorT &value;

    AsNormal(const VectorT &value)
        : value(value)
    {
    }
};

template <typename Scalar, int Dim, int Mode, typename Vec>
auto operator*(const Eigen::Transform<Scalar, Dim, Mode, 0> &transform,
    const AsVector<Vec> &vector) -> Eigen::Matrix<Scalar, Dim, 1>
{
    Eigen::Matrix<Scalar, Dim + 1, 1> affine_vec;
    affine_vec.template head<Dim>() = vector.value;
    affine_vec(Dim) = 0;
    return (transform * affine_vec).template head<Dim>();
}

template <typename Scalar, int Dim, int Mode, typename Vec>
auto operator*(const Eigen::Transform<Scalar, Dim, Mode, 0> &transform,
    const AsPoint<Vec> &point) -> Eigen::Matrix<Scalar, Dim, 1>
{
    return (transform * point.value.homogeneous()).template head<Dim>();
}
}
