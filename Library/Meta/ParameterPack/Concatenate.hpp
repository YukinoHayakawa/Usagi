#pragma once

#include <type_traits>

#include <Usagi/Library/Meta/TypeContainers/TypeList.hpp>

namespace usagi::meta
{
// Concatenate parameter lists of two instantiations of the same list-like type
// together.
template <typename Lhs, typename Rhs>
struct Concatenate;

template <
    template <typename...> typename Tuple,
    typename... Ls,
    typename... Rs
>
struct Concatenate<Tuple<Ls...>, Tuple<Rs...>>
{
    using type = Tuple<Ls..., Rs...>;
};

template <typename Lhs, typename Rhs>
using Concatenated = typename Concatenate<Lhs, Rhs>::type;

static_assert(std::is_same_v<
    Concatenated<TypeList<int, bool>, TypeList<bool, double>>, 
    TypeList<int, bool, bool, double>
>);

/*
// Since duplicated base classes is ill-formed, we can know are there
// duplicated elements in the list. -- for some reasons this doesn't work.
template <typename... Ts>
concept NoDuplicatedElements = requires {
    typename detail::ElementTrain<Ts...>::type;
};
*/
}
