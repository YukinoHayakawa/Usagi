#pragma once

#include <functional>

#include "ImGuiComponent.hpp"

namespace usagi
{
struct DelegatedImGuiComponent : ImGuiComponent
{
    using DrawFunction = std::function<void(const Clock &clock)>;
    DrawFunction draw_func;

    DelegatedImGuiComponent() = default;

    explicit DelegatedImGuiComponent(
        DrawFunction draw_func)
        : draw_func(std::move(draw_func))
    {
    }

    void draw(const Clock &clock) override
    {
        draw_func(clock);
    }
};
}
