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
 * Uniqueness of the component types is ensured by that having the same
 * class as a direct base class for multiple times is not allowed.
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

    constexpr ComponentFilter() = default;

    constexpr ComponentFilter(Components &&...)
        requires (sizeof...(Components) > 0)
    {
    }
};

// Shortcut name
// bug: waiting for P1814R0 to be implemented
template <Component... Components>
using C = struct ComponentFilter<Components...>;

// CTAD guide
// template <typename...>
// ComponentFilter() -> ComponentFilter<>;

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
