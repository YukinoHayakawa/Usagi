#pragma once

#include <Usagi/Core/Component.hpp>

#include "DebugDraw.hpp"

namespace usagi
{
struct DebugDrawComponent : Component
{
    virtual void draw(dd::ContextHandle ctx) = 0;

    const std::type_info & baseType() override final
    {
        return typeid(DebugDrawComponent);
    }
};
}
