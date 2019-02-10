#pragma once

#include <Usagi/Core/Math.hpp>
#include <Usagi/Utility/Math.hpp>

namespace usagi
{
struct Ray
{
    Vector3f origin;
    Vector3f direction;
    Interval<float> t_range { 0.f, std::numeric_limits<float>::infinity() };

    Vector3f operator()(const float t) const
    {
        return origin + t * direction;
    }
};
}
