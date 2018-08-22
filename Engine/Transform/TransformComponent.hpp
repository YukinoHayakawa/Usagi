#pragma once

#include <Usagi/Engine/Core/Component.hpp>
#include <Usagi/Engine/Core/Math.hpp>

namespace usagi
{
// todo animated transform?
struct TransformComponent : Component
{
    Affine3f local_to_parent = Affine3f::Identity();

    /**
     * \brief Transform from local coordinates to world coordinates.
     * Calculated by TransformSubsystem using
     * <Parent Element>.local_to_world * local_to_world if the parent has
     * a TransformComponent. Otherwise identical to local_to_parent.
     * The calculation cascade towards the root element and is cached if
     * local_to_parent is not changed.
     */
    Affine3f local_to_world = Affine3f::Identity();

    /**
     * \brief A flag indicating whether local_to_world should be re-calculated.
     * This should be set whenever local_to_parent is modified.
     */
    bool needs_update = false;

    const std::type_info & baseType() override
    {
        return typeid(TransformComponent);
    }
};
}
