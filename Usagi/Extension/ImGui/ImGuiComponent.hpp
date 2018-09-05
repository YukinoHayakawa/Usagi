#pragma once

#include <Usagi/Core/Component.hpp>

namespace usagi
{
class Clock;

struct ImGuiComponent : Component
{
    virtual void draw(const Clock &clock) = 0;

    const std::type_info & baseType() override final
    {
        return typeid(ImGuiComponent);
    }
};
}
