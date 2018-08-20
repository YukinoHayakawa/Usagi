#pragma once

#include <Usagi/Engine/Core/Time.hpp>
#include <Usagi/Engine/Core/Component.hpp>

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
