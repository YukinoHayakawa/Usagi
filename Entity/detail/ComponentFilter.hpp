#pragma once

#include <type_traits>

#include <Usagi/Entity/Component.hpp>
#include <Usagi/Library/Meta/List.hpp>
#include <Usagi/Library/Meta/Tag.hpp>

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

template <typename Lhs, typename Rhs>
struct FilterDeduplicateHelper;

template <Component... Cs>
struct FilterDeduplicateHelper<meta::List<Cs...>, meta::List<>>
{
    using type = ComponentFilter<Cs...>;
};

template <Component... Cs, Component D, Component... Ds>
struct FilterDeduplicateHelper<meta::List<Cs...>, meta::List<D, Ds...>>
{
    using type = typename FilterDeduplicateHelper<
        std::conditional_t<
            ComponentFilter<Cs...>::template HAS_COMPONENT<D>,
            meta::List<Cs...>,
            meta::List<Cs..., D>
        >,
        meta::List<Ds...>
    >::type;
};

template <typename Filter>
struct FilterDeduplicated;

template <Component... Cs>
struct FilterDeduplicated<ComponentFilter<Cs...>>
{
    using type = typename FilterDeduplicateHelper<
        meta::List<>, meta::List<Cs...>
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
