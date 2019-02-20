#pragma once

#include <optional>

#include <Usagi/Game/CollectionSubsystem.hpp>

#include "ShapeComponent.hpp"
#include "Intersection.hpp"

namespace usagi
{
struct Ray;

/**
 * \brief Provides ray cast service. Does nothing during update.
 */
class RayCastSubsystem : public CollectionSubsystem<ShapeComponent>
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
