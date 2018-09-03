#pragma once

#include "Camera.hpp"

namespace usagi
{
class PerspectiveCamera : public Camera
{
    Projective3f mLocalToNdc;

public:
    PerspectiveCamera() = default;
    PerspectiveCamera(float fov_y_radians, float aspect, float near, float far);

    void setPerspective(float fov_y_radians, float aspect, float near, float far);

    Projective3f localToNDC() const override { return mLocalToNdc; }
};
}
