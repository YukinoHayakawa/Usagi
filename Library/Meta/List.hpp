#pragma once

namespace usagi::meta
{
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

template <typename... Cs>
struct List
{
};

/*
// Since duplicated base classes is ill-formed, we can know are there
// duplicated elements in the list. -- for some reasons this doesn't work.
template <typename... Ts>
concept NoDuplicatedElements = requires {
    typename detail::ElementTrain<Ts...>::type;
};
*/
}
