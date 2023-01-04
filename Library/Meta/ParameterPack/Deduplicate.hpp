#pragma once

#include <Usagi/Library/Meta/TypeContainers/TypeSet.hpp>

namespace usagi::meta
{
template <typename Lhs, typename Rhs>
struct DeduplicateHelper;

template<typename... Lhs>
struct DeduplicateHelper<TypeSet<Lhs...>, TypeSet<>>
{
    using type = TypeSet<Lhs...>;
};

template<typename... Lhs, typename R, typename... Rhs>
struct DeduplicateHelper<TypeSet<Lhs...>, TypeSet<R, Rhs...>>
{
    using type = typename DeduplicateHelper<
        typename TypeSet<Lhs...>::template insert<R>,
        TypeSet<Rhs...>
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
        TypeSet<>, TypeSet<Ts...>
    >::type::template apply<Tuple>;
};

template <typename TypeListT>
using Deduplicated = typename Deduplicate<TypeListT>::type;
}
