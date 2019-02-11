#pragma once

#include <memory>

#include <Usagi/Core/Component.hpp>

namespace usagi
{
class Shape;

struct GeometryComponent : Component
{
    std::shared_ptr<Shape> shape;

    GeometryComponent(std::shared_ptr<Shape> shape)
        : shape(std::move(shape))
    {
    }

    const std::type_info & baseType() override
    {
        return typeid(GeometryComponent);
    }
};
}
