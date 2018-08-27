#pragma once

#include <Usagi/Core/Time.hpp>
#include <Usagi/Core/Component.hpp>

namespace usagi
{
struct ImGuiComponent : Component
{
    virtual void draw(const TimeDuration &dt) = 0;

    const std::type_info & baseType() override final
    {
        return typeid(ImGuiComponent);
    }
};
}
