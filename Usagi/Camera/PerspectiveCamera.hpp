#pragma once

#include "ProjectiveCamera.hpp"

namespace usagi
{
class PerspectiveCamera : public ProjectiveCamera
{
public:
    void setPerspective(
        const Vector2f &screen_size,
        float fov_y_radians,
        float aspect,
        float near,
        float far);

    Ray generateRay(const CameraSample &sample) const override;
};
}
