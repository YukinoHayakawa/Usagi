#include "RayCastSystem.hpp"

#include "Intersection.hpp"
#include "Shape.hpp"
#include "Ray.hpp"

std::optional<usagi::Intersection> usagi::RayCastSystem::intersect(
    const Ray &ray)
{
    Intersection x;
    // todo make it more efficient
    for(auto e : mRegistry)
    {
        if(std::get<ShapeComponent*>(e.second)->shape->intersect(ray, x))
        {
            ray.t_range.max = x.distance;
            x.element = e.first;
        }
    }

    if(x.shape)
        return x;
    return { };
}
