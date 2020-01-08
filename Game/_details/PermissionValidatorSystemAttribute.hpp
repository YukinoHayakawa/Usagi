#pragma once

#include <Usagi/Experimental/v2/Game/Entity/Component.hpp>

namespace usagi
{
/**
 * \brief Grants access to systems if they have the according permission.
 * \tparam ReadAccessFilter An instantiation of ComponentFilter containing the
 * components for which the system has read access.
 * \tparam WriteAccessFilter An instantiation of ComponentFilter containing the
 * components for which the system has write access.
 */
template <
    typename ReadAccessFilter,
    typename WriteAccessFilter
>
struct PermissionValidatorSystemAttribute
{
    template <Component C>
    static constexpr bool hasReadAccess()
    {
        return ReadAccessFilter::template hasComponent<C>() ||
            WriteAccessFilter::template hasComponent<C>();
    }

    template <Component C>
    static constexpr bool hasWriteAccess()
    {
        return WriteAccessFilter::template hasComponent<C>();
    }
};
}
