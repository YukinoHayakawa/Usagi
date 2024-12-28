#pragma once

#include <Usagi/Library/Meta/Types/Containers/List/TypeList.hpp>

namespace usagi::meta
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
        containers::TypeList<int>(),
        containers::TypeList<char>()
    )),
    containers::TypeList<int, char>
>);
static_assert(std::is_same_v<
    decltype(concatenate_lists(
        containers::TypeList<int>(),
        containers::TypeList<>()
    )),
    containers::TypeList<int>
>);
static_assert(std::is_same_v<
    decltype(concatenate_lists(
        containers::TypeList<int>(),
        containers::TypeList<char>(),
        containers::TypeList<bool>()
    )),
    containers::TypeList<int, char, bool>
>);
}
