#pragma once

#include <Usagi/Engine/Core/Time.hpp>
#include <Usagi/Engine/Core/Component.hpp>

namespace usagi
{
struct ImGuiComponent : Component
{
    virtual void draw(const TimeDuration &dt) = 0;

    const std::type_info & getBaseTypeInfo() override
    {
        return typeid(ImGuiComponent);
    }
};
}
