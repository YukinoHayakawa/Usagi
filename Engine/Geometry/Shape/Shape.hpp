#pragma once

#include <Usagi/Engine/Geometry/Ray.hpp>
#include <Usagi/Engine/Geometry/Intersection.hpp>

namespace yuki
{
class Shape
{
public:
    virtual ~Shape() = default;

    /**
     * \brief Determine whether the shape intersects with a given ray.
     * \param ray 
     * \return 
     */
    virtual bool intersect(const Ray &ray) { return false; }

    /**
     * \brief Find the nearest intersection of a given ray with the shape.
     * \param ray 
     * \param x 
     * \return 
     */
    virtual bool intersect(const Ray &ray, Intersection &x) { return false; }
};
}
