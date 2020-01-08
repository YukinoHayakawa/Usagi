#pragma once

#include <Usagi/Experimental/v2/Game/Entity/Component.hpp>

namespace usagi
{
struct PermissionValidatorReadOnly
{
    template <Component C>
    static constexpr bool hasReadAccess()
    {
        return true;
    }

    template <Component C>
    static constexpr bool hasWriteAccess()
    {
        return false;
    }
};
}
