#pragma once

#include <type_traits>

#include "TypeTag.hpp"

namespace usagi::meta
{
template <typename... Ts>
struct TypeSet : TypeTag<Ts>...
{
    template <typename T>
    using insert = std::conditional_t<
        std::is_base_of_v<TypeTag<T>, TypeSet>,
        TypeSet<Ts...>,
        TypeSet<Ts..., T>
    >;

    template <template <typename...> typename T>
    using apply = T<Ts...>;
};
}
