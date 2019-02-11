#pragma once

#include <functional>

#include "DebugDrawComponent.hpp"

namespace usagi
{
struct DelegatedDebugDrawComponent : DebugDrawComponent
{
    using DrawFunction = std::function<void(dd::ContextHandle ctx)>;
    DrawFunction draw_func;

    DelegatedDebugDrawComponent() = default;

    DelegatedDebugDrawComponent(DrawFunction draw_func)
        : draw_func(std::move(draw_func))
    {
    }

    void draw(dd::ContextHandle ctx) override
    {
        draw_func(ctx);
    }
};
}
