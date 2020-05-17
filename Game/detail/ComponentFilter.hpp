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
    using rebind = T<Components...>;

    template <
        template <typename...>
        typename T,
        template <typename>
        typename N
    >
    using rebind_to_template = T<N<Components>...>;
};

// Component Filter Deduplication

template <typename Lhs, typename Rhs>
struct FilterDeduplicateHelper;

template <Component... Cs>
struct FilterDeduplicateHelper<ComponentFilter<Cs...>, ComponentFilter<>>
{
    using type = ComponentFilter<Cs...>;
};

template <Component... Cs, Component D, Component... Ds>
struct FilterDeduplicateHelper<
        ComponentFilter<Cs...>,
        ComponentFilter<D, Ds...>
    >
{
    using type = std::conditional_t<
        ComponentFilter<Cs...>::template HAS_COMPONENT<D>,
        typename FilterDeduplicateHelper<
            ComponentFilter<Cs...>, ComponentFilter<Ds...>
        >::type,
        typename FilterDeduplicateHelper<
            ComponentFilter<Cs..., D>, ComponentFilter<Ds...>
        >::type
    >;
};

template <typename Filter>
struct FilterDeduplicated;

template <Component... Cs>
struct FilterDeduplicated<ComponentFilter<Cs...>>
{
    using type = typename FilterDeduplicateHelper<
        ComponentFilter<>, ComponentFilter<Cs...>
    >::type;
};

template <typename Filter>
using FilterDeduplicatedT = typename FilterDeduplicated<Filter>::type;

template <Component... Cs>
using UniqueComponents = FilterDeduplicatedT<ComponentFilter<Cs...>>;

// Component Filter Concatenation

template <typename Lhs, typename Rhs>
struct FilterConcatenated;

template <Component... Cs, Component... Ds>
struct FilterConcatenated<ComponentFilter<Cs...>, ComponentFilter<Ds...>>
{
    using type = ComponentFilter<Cs..., Ds...>;
};

template <typename Lhs, typename Rhs>
using FilterConcatenatedT = typename FilterConcatenated<Lhs, Rhs>::type;
}
