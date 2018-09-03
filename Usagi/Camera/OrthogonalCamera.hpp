#pragma once

#include "Camera.hpp"

namespace usagi
{
class OrthogonalCamera : public Camera
{
    Projective3f mLocalToNdc;

public:
    OrthogonalCamera() = default;

    void setOrthogonal(
        float left,
        float right,
        float bottom,
        float top,
        float near,
        float far);

    Projective3f localToNDC() const override { return mLocalToNdc; }
};
}
