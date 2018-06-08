#include "Sphere.hpp"

yuki::Sphere::Sphere(const float radius)
    : mRadius { radius }
{
}

// Jeff Hultquist, Intersection of a Ray with a Sphere, Graphics Gems, pp. 388 - 389.
bool yuki::Sphere::intersect(const Ray &ray)
{
    const auto v = -ray.origin.dot(ray.direction);
    const auto disc = mRadius * mRadius - ray.origin.dot(ray.origin) - v * v;
    return disc >= 0;
}

bool yuki::Sphere::intersect(const Ray &ray, Intersection &x)
{
    return false;
}
