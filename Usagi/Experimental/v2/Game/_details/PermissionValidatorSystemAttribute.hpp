#pragma once

#include <Usagi/Experimental/v2/Game/Entity/Component.hpp>

namespace usagi
{
/**
 * \brief Grants access to systems if they have the according permission.
 * \tparam ReadMask An instantiation of ComponentFilter containing the
 * components for which the system has read access.
 * \tparam WriteMask An instantiation of ComponentFilter containing the
 * components for which the system has write access.
 */
template <
    typename ReadMask,
    typename WriteMask
>
struct PermissionValidatorSystemAttribute
{
    template <Component C>
    static constexpr bool hasReadAccess()
    {
        return ReadMask::template hasComponent<C>() ||
            WriteMask::template hasComponent<C>();
    }

    template <Component C>
    static constexpr bool hasWriteAccess()
    {
        return WriteMask::template hasComponent<C>();
    }
};
}
