#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <Usagi/Engine/Entity/Component.hpp>

namespace usagi
{
struct TransformComponent : Component
{
    Eigen::Vector3f position;
    Eigen::Vector3f scaling;
    Eigen::Quaternionf rotation;

    Eigen::Affine3f getModelToWorldMatrix() const
    {
        Eigen::Affine3f model_to_world = Eigen::Affine3f::Identity();
        model_to_world.rotate(rotation)
             .scale(scaling)
             .translate(position);
        return model_to_world;
    }
};
}
