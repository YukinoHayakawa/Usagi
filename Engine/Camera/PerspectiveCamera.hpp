#pragma once

#include <Usagi/Engine/Geometry/Angle.hpp>
#include "Camera.hpp"

namespace usagi
{

class PerspectiveCamera : public Camera
{
    Eigen::Projective3f mLocalToNdc = Eigen::Projective3f::Identity();

public:
    void setPerspective(Radians fovY, float aspect, float near, float far);

    Eigen::Projective3f getProjectionMatrix() override
    {
        return mLocalToNdc;
    }
};

}
