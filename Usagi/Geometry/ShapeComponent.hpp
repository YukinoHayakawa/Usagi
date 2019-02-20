#pragma once

#include <memory>

#include <Usagi/Core/Component.hpp>

namespace usagi
{
class Shape;

struct ShapeComponent : Component
{
    std::shared_ptr<Shape> shape;

    ShapeComponent(std::shared_ptr<Shape> shape)
        : shape(std::move(shape))
    {
    }

    const std::type_info & baseType() override
    {
        return typeid(ShapeComponent);
    }
};
}
