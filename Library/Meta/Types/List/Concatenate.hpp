#pragma once

#include <Usagi/Library/Meta/Types/List/TypeList.hpp>

namespace usagi::meta::types
{
consteval auto concatenate_lists(auto &&list, auto &&...lists)
    requires (sizeof...(lists) > 0)
{
    if constexpr(sizeof...(lists) == 1)
    {
        constexpr auto op = []<
            template <typename...> typename List,
            typename... Ts,
            typename... Us
        >(List<Ts...>, List<Us...>)
        {
            return List<Ts..., Us...>();
        };
        return op(list, lists...[0]);
    }
    else
    {
        return concatenate_lists(list, concatenate_lists(lists...));
    }
}

/*
 * Static Tests
 */

static_assert(std::is_same_v<
    decltype(concatenate_lists(
        TypeList<int>(),
        TypeList<char>()
    )),
    TypeList<int, char>
>);
static_assert(std::is_same_v<
    decltype(concatenate_lists(
        TypeList<int>(),
        TypeList<>()
    )),
    TypeList<int>
>);
static_assert(std::is_same_v<
    decltype(concatenate_lists(
        TypeList<int>(),
        TypeList<char>(),
        TypeList<bool>()
    )),
    TypeList<int, char, bool>
>);
}
