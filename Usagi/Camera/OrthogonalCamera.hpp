#pragma once

#include "ProjectiveCamera.hpp"

namespace usagi
{
class OrthogonalCamera : public ProjectiveCamera
{
public:
    void setOrthogonal(
        float left,
        float right,
        float bottom,
        float top,
        float near,
        float far);
};
}
