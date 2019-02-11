#pragma once

#include "Camera.hpp"

namespace usagi
{
class ProjectiveCamera : public Camera
{
protected:
    // transform from camera local to normalized device coordinate cube
    Projective3f mLocalToNdc;
    // transform from screen space (framebuffer size with depth) to
    // camera local
    Projective3f mScreenToLocal;

public:
    Projective3f localToNDC() const override { return mLocalToNdc; }
};
}
