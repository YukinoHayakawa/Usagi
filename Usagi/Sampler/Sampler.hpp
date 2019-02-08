#pragma once

#include <Usagi/Core/Math.hpp>

namespace usagi
{
class Sampler
{
public:
    virtual ~Sampler() = default;

    virtual float next1D() = 0;
    virtual Vector2f next2D() = 0;
    virtual Vector3f next3D() = 0;
};
}
