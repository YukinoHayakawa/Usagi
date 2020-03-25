#pragma once

#include <Usagi/Game/Entity/Component.hpp>

namespace usagi
{
/**
 * \brief Contains a list of component types which may be used to filter
 * entities or check access permissions.
 * todo ensure uniqueness of the component types
 * \tparam Components A sequence of component types.
 */
template <Component... Components>
struct ComponentFilter
{
    template <Component C>
    static constexpr bool HAS_COMPONENT =
        std::disjunction_v<std::is_same<C, Components>...>;

    template <
        template <typename...>
        typename T
    >
    using Apply = T<Components...>;

    template <
        template <typename...>
        typename T,
        template <typename>
        typename N
    >
    using NestedApply = T<N<Components>...>;
};
}
