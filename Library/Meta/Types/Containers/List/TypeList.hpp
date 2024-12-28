#pragma once

namespace usagi::meta::containers
{
template <typename... Ts>
struct TypeList
{
    template <template <typename...> typename T>
    using rebind = T<Ts...>;
};
}
