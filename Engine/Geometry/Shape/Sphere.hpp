#pragma once

#include "Shape.hpp"

namespace yuki
{
class Sphere : public Shape
{
    float mRadius = 1.f;

public:
    Sphere() = default;
    explicit Sphere(float radius);

    bool intersect(const Ray &ray) override;
    bool intersect(const Ray &ray, Intersection &x) override;
};
}
