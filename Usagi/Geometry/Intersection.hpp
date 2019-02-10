#pragma once

#include <Usagi/Core/Math.hpp>

namespace usagi
{
struct Intersection
{
    Vector3f position;
    float distance = std::numeric_limits<float>::infinity();
    Vector3f normal;
    bool inside = false;
};
}
