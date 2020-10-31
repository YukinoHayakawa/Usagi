#pragma once

#include "Set.hpp"

namespace usagi::meta
{
template <typename Lhs, typename Rhs>
struct DeduplicateHelper;

template<typename... Lhs>
struct DeduplicateHelper<Set<Lhs...>, Set<>>
{
    using type = Set<Lhs...>;
};

template<typename... Lhs, typename R, typename... Rhs>
struct DeduplicateHelper<Set<Lhs...>, Set<R, Rhs...>>
{
    using type = typename DeduplicateHelper<
        typename Set<Lhs...>::template insert<R>,
        Set<Rhs...>
    >::type;
};

template <typename List>
struct Deduplicate;

template <
    template <typename...> typename Tuple,
    typename... Ts
>
struct Deduplicate<Tuple<Ts...>>
{
    using type = typename DeduplicateHelper<
        Set<>, Set<Ts...>
    >::type::template apply<Tuple>;
};

template <typename... Ts>
using Deduplicated = typename Deduplicate<Set<Ts...>>::type;
}
