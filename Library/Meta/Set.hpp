#pragma once

#include <type_traits>

#include "Tag.hpp"

namespace usagi::meta
{
template <typename... Ts>
struct Set : Tag<Ts>...
{
    template <typename T>
    using insert = std::conditional_t<
        std::is_base_of_v<Tag<T>, Set>,
        Set<Ts...>,
        Set<Ts..., T>
    >;

    template <template <typename...> typename T>
    using apply = T<Ts...>;
};
}
