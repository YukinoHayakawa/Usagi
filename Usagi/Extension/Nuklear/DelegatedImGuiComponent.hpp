#pragma once

#include <functional>

#include "NuklearComponent.hpp"

namespace usagi
{
struct DelegatedNuklearComponent : NuklearComponent
{
    using DrawFunction =
        std::function<void(const Clock &clock, nk_context *ctx)>;
    DrawFunction draw_func;

    explicit DelegatedNuklearComponent(DrawFunction draw_func)
        : draw_func(std::move(draw_func))
    {
    }

    void draw(const Clock &clock, nk_context *ctx) override
    {
        draw_func(clock,ctx);
    }
};
}
