#pragma once

#include <Usagi/Core/Component.hpp>

namespace usagi
{
// tags ray-cast-able elements
struct RayCastComponent : Component
{
    const std::type_info & baseType() override
    {
        return typeid(RayCastComponent);
    }
};
}
