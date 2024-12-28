#pragma once

namespace usagi::meta
{
template <template <typename...> typename List, typename... Ts, typename... Us>
consteval auto concatenate_lists(List<Ts...>, List<Us...>)
{
    return List<Ts..., Us...>();
}
}
