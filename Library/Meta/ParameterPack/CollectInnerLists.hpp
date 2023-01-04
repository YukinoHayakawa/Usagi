#pragma once

#include "Concatenate.hpp"

namespace usagi::meta
{
/*
template <typename T>
struct ExampleTrait
{
    using InnerListT = typename T::ReadAccess;
};
*/

// Collect the parameters of list-like type containers inside a list of other
// types together.
template <template <typename> typename Trait, typename... Ts>
struct CollectInnerList;

template <template <typename> typename Trait, typename T, typename... Ts>
struct CollectInnerList<Trait, T, Ts...>
{
    using type = typename Concatenate<
        typename Trait<T>::InnerListT,
        typename CollectInnerList<Trait, Ts...>::type
    >::type;
};

template <template <typename> typename Trait, typename T>
struct CollectInnerList<Trait, T>
{
    using type = typename Trait<T>::InnerListT;
};

template <template <typename> typename Trait, typename... Ts>
using CollectedInnerList = typename CollectInnerList<Trait, Ts...>::type;
}
