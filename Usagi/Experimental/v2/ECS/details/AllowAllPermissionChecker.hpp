#pragma once

#include "../Component.hpp"

namespace usagi::ecs
{
/**
 * \brief Grants access to all components. Mainly for test purposes or internal
 * uses.
 */
struct AllowAllPermissionChecker
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
