﻿#pragma once

#include <Usagi/Geometry/Shape.hpp>
#include <Usagi/Math/Matrix.hpp>

namespace usagi
{
class Sphere : public Shape
{
    Vector3f mCenter = Vector3f::Zero();
    float mRadius = 1.f;

    template <bool FillRecord>
    bool intersect(const Ray &ray, Intersection *x);

public:
    Sphere() = default;
    Sphere(Vector3f center, float radius);

    bool intersect(const Ray &ray) override;
    bool intersect(const Ray &ray, Intersection &x) override;

    const Vector3f & center() const
    {
        return mCenter;
    }

    float radius() const
    {
        return mRadius;
    }

    void setCenter(Vector3f center)
    {
        mCenter = std::move(center);
    }

    void setRadius(float radius)
    {
        mRadius = radius;
    }
};
}
