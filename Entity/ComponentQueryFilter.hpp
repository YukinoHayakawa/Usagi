#pragma once

#include "detail/ComponentFilter.hpp"

namespace usagi
{
template <
    SimpleComponentFilter IncludeFilter,
    SimpleComponentFilter ExcludeFilter = C<>
>
struct ComponentQueryFilter
{
    using IncludeFilter = IncludeFilter;
    using ExcludeFilter = ExcludeFilter;

    using ReadAccess = CatComponentFiltersUnique<IncludeFilter, ExcludeFilter>;
};

template <
    SimpleComponentFilter IncludeFilter,
    SimpleComponentFilter ExcludeFilter = C<>
>
using CQuery = ComponentQueryFilter<IncludeFilter, ExcludeFilter>;

template <typename T>
concept SimpleComponentQuery = requires
{
    typename T::IncludeFilter;
    typename T::ExcludeFilter;
    typename T::ReadAccess;
} && SimpleComponentFilter<typename T::IncludeFilter>
  && SimpleComponentFilter<typename T::ExcludeFilter>;

template <typename T>
using QueryT = typename T::QueryT;

template<SimpleComponentQuery Query>
using QueryReadAccess = typename Query::ReadAccess;

template <SimpleComponentQuery Query, Component... Comp>
constexpr bool QueryIncludesComponent =
    (Query::IncludeFilter::template HasComponent<Comp> && ...);

template <SimpleComponentQuery Query, Component... Comp>
constexpr bool QueryExcludesComponent =
    (Query::ExcludeFilter::template HasComponent<Comp> && ...);
}
