#pragma once

#include <Usagi/Core/Math.hpp>
#include <Usagi/Utility/Math.hpp>

#include "Transform.hpp"

namespace usagi
{
// parametric representation of a ray
struct Ray
{
    Vector3f origin;
    Vector3f direction;
    // t is the parameter
    mutable Interval<float> t_range {
        std::numeric_limits<float>::epsilon(),
        std::numeric_limits<float>::infinity()
    };

    Vector3f operator()(const float t) const
    {
        return origin + t * direction;
    }
};

template <typename Scalar, int Dim, int Mode>
auto operator*(const Eigen::Transform<Scalar, Dim, Mode> &transform,
    const Ray &ray)
{
    Ray r;
    r.origin = transform * Point(ray.origin);
    r.direction = transform * Vector(ray.direction);
    return r;
}
}
