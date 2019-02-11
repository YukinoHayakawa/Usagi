#include "RayCastSubsystem.hpp"

#include "Intersection.hpp"
#include "Shape.hpp"
#include "Ray.hpp"

std::optional<usagi::Intersection> usagi::RayCastSubsystem::intersect(
    const Ray &ray)
{
    Intersection x;
    for(auto e : mRegistry)
    {
        if(std::get<GeometryComponent*>(e.second)->shape->intersect(ray, x))
        {
            ray.t_range.max = x.distance;
            x.element = e.first;
        }
    }

    if(x.shape)
        return x;
    return { };
}
