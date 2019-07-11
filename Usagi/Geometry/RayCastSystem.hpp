#pragma once

#include <optional>

#include <Usagi/Game/CollectionSystem.hpp>

#include "ShapeComponent.hpp"
#include "Intersection.hpp"
#include "RayCastComponent.hpp"

namespace usagi
{
struct Ray;

/**
 * \brief Provides ray cast service. Does nothing during update.
 */
class RayCastSystem
    : public CollectionSystem<
        ShapeComponent,
        RayCastComponent
    >
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
