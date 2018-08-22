#pragma once

#include <memory>

#include <Usagi/Engine/Core/Component.hpp>

namespace usagi
{
class Camera;

struct CameraComponent : Component
{
    std::shared_ptr<Camera> camera;

    CameraComponent(std::shared_ptr<Camera> camera)
        : camera(std::move(camera))
    {
    }

    const std::type_info & baseType() override
    {
        return typeid(CameraComponent);
    }
};
}
