#pragma once

#include <Usagi/Experimental/v2/Game/Entity/Component.hpp>

namespace usagi
{
/**
 * \brief Grants access to all components. Mainly for test purposes or internal
 * uses.
 */
struct PermissionValidatorAllowAll
{
    template <Component C>
    static constexpr bool hasReadAccess()
    {
        return true;
    }

    template <Component C>
    static constexpr bool hasWriteAccess()
    {
        return true;
    }
};
}
