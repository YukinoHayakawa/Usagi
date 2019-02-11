#include "Sphere.hpp"

#include <Usagi/Geometry/Ray.hpp>
#include <Usagi/Geometry/Intersection.hpp>
#include <Usagi/Utility/Math.hpp>

usagi::Sphere::Sphere(Vector3f center, float radius)
    : mCenter(std::move(center))
    , mRadius(radius)
{
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection

template <bool FillRecord>
bool usagi::Sphere::intersect(const Ray &ray, Intersection *x)
{
    const auto a = ray.direction.dot(ray.direction);
    const Vector3f d_oc = ray.origin - mCenter;
    const auto b = 2.f * d_oc.dot(ray.direction);
    const auto c = d_oc.dot(d_oc) - mRadius * mRadius;

    const auto [disc, t0, t1] = solveQuadratic(a, b, c);

    // no intersection
    if(disc < 0)
        return false;

    if constexpr(FillRecord)
    {
        if(ray.t_range.openContains(t0))
        {
            x->position = ray(t0);
            x->distance = t0;
            x->inside = false;
        }
        else if(ray.t_range.openContains(t1))
        {
            x->position = ray(t1);
            x->distance = t1;
            x->inside = true;
        }
        else
        {
            return false;
        }
        x->normal = (x->position - mCenter).normalized();
        x->shape = this;

        return true;
    }
    else
    {
        return ray.t_range.openContains(t0) || ray.t_range.openContains(t1);
    }
}

bool usagi::Sphere::intersect(const Ray &ray)
{
    return intersect<false>(ray, nullptr);
}

bool usagi::Sphere::intersect(const Ray &ray, Intersection &x)
{
    return intersect<true>(ray, &x);
}
