#pragma once

#include "../Component.hpp"

namespace usagi::ecs
{
/**
 * \brief Grants access to systems if they have the according permission.
 * \tparam ReadMask An instantiation of ComponentFilter containing the
 * components for which the system has read acces.
 * \tparam WriteMask An instantiation of ComponentFilter containing the
 * components for which the system has write access.
 */
template <
    typename ReadMask,
    typename WriteMask
>
struct SystemPermissionChecker
{
    template <Component C>
    static constexpr bool hasReadAccess()
    {
        return ReadMask::hasComponent<C>() || WriteMask::hasComponent<C>();
    }

    template <Component C>
    static constexpr bool hasWriteAccess()
    {
        return WriteMask::hasComponent<C>();
    }
};
}
