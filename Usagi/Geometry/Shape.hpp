#pragma once

namespace usagi
{
struct Intersection;
struct Ray;

class Shape
{
public:
    virtual ~Shape() = default;

    /**
     * \brief Determine whether the shape intersects with a given ray.
     * \param ray Test ray in local coordinates.
     * \return
     */
    virtual bool intersect(const Ray &ray) { return false; }

    /**
     * \brief Find the nearest intersection of a given ray with the shape.
     * \param ray Test ray in local coordinates.
     * \param x The record to be updated if any intersection was found.
     * \return
     */
    virtual bool intersect(const Ray &ray, Intersection &x) { return false; }
};
}
