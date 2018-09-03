#include "OrthogonalCamera.hpp"

void usagi::OrthogonalCamera::setOrthogonal(
    const float left,
    const float right,
    const float bottom,
    const float top,
    const float near,
    const float far)
{
    Matrix4f mat;
    mat <<
        2 / (right - left), 0, 0, -(right + left) / (right - left),
        0, 0, -2 / (top - bottom), (top + bottom) / (top - bottom),
        0, 1 / (far - near), 0, -near / (far - near),
        0, 0, 0, 1;

    mLocalToNdc = mat;
}
