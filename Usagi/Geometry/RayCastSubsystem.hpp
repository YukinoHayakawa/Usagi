#pragma once

#include <optional>

#include <Usagi/Game/CollectionSubsystem.hpp>

#include "GeometryComponent.hpp"
#include "Intersection.hpp"

namespace usagi
{
struct Ray;

/**
 * \brief Provides ray cast service. Does nothing during update.
 */
class RayCastSubsystem : public CollectionSubsystem<GeometryComponent>
{
public:
    void update(const Clock &clock) override { }

    /**
     * \brief
     * \param ray Ray in world coordinates.
     * \return
     */
    std::optional<Intersection> intersect(const Ray &ray);
};
}
