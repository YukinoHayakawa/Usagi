#pragma once

#include <Usagi/Engine/Core/Math.hpp>

namespace usagi
{
inline Affine3f lookAt(
    const Vector3f &from,
    const Vector3f &to = Vector3f::Zero(),
    const Vector3f &up = Vector3f::UnitZ())
{
    const auto y_front = (to - from).normalized();
    const auto x_right = y_front.cross(up);
    const auto z_up = x_right.cross(y_front);

    Matrix3f rot;
    rot << x_right, y_front, z_up;

    auto transform = Affine3f::Identity();
    transform.rotate(rot);
    transform.translate(from);

    return transform;
}
}
