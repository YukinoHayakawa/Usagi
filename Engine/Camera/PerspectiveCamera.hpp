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

    void setMatrix(float fov_y_radians, float aspect, float near, float far);

    Projective3f projectionMatrix() const override { return mLocalToNdc; }
};
}
