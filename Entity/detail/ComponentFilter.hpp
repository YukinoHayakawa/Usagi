#pragma once

#include <type_traits>

#include <Usagi/Entity/Component.hpp>
#include <Usagi/Library/Meta/Deduplicate.hpp>
#include <Usagi/Library/Meta/List.hpp>

namespace usagi
{
/**
 * \brief Contains a list of component types which may be used to filter
 * entities or check access permissions.
 * todo ensure uniqueness of the component types
 * \tparam Components A sequence of component types.
 */
template <Component... Components>
struct ComponentFilter : Tag<Components>...
{
    template <Component C>
    static constexpr bool HAS_COMPONENT = std::is_base_of_v<
        Tag<C>,
        ComponentFilter
    >;

    template <
        template <typename...>
        typename T
    >
    using apply = T<Components...>;

    template <
        template <typename...>
        typename T,
        template <typename>
        typename N
    >
    using apply_with = T<N<Components>...>;
};

// Component Filter Deduplication
template <typename Filter>
using FilterDeduplicatedT = typename meta::Deduplicate<Filter>::type;

template <Component... Cs>
using UniqueComponents =
    typename meta::Deduplicate<ComponentFilter<Cs...>>::type;

// Component Filter Concatenation
template <typename Lhs, typename Rhs>
using FilterConcatenatedT = typename meta::Concatenate<Lhs, Rhs>::type;
}
