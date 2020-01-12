#pragma once

#include <Usagi/Experimental/v2/Game/Entity/Component.hpp>

namespace usagi
{
/**
 * \brief Contains a list of component types which may be used to filter
 * entities or check access permissions.
 * \tparam Components A sequence of component types.
 */
template <Component... Components>
struct ComponentFilter
{
    template <Component C>
    static constexpr bool HAS_COMPONENT =
        std::disjunction_v<std::is_same<C, Components>...>;
};
}
