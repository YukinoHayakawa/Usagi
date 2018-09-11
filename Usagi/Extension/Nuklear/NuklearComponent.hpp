#pragma once

#include <Usagi/Core/Component.hpp>

#include "Nuklear.hpp"

namespace usagi
{
class Clock;

struct NuklearComponent : Component
{
    virtual void draw(const Clock &clock, nk_context *ctx) = 0;

    const std::type_info & baseType() override final
    {
        return typeid(NuklearComponent);
    }
};
}
