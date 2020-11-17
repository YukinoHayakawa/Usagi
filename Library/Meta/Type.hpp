#pragma once

#include <type_traits>

namespace usagi
{
template <
    template <typename...> typename Template,
    bool Condition,
    typename Otherwise,
    typename... Ts
>
struct ConditionalApply;

template <
    template <typename...> typename Template,
    typename Otherwise,
    typename... Ts
>
struct ConditionalApply<Template, true, Otherwise, Ts...>
{
    using type = Template<Ts...>;
};

template <
    template <typename...> typename Template,
    typename Otherwise,
    typename... Ts
>
struct ConditionalApply<Template, false, Otherwise, Ts...>
{
    using type = Otherwise;
};

template <
    template <typename...> typename Template,
    bool Condition,
    typename Otherwise,
    typename... Ts
>
using ConditionalApplyT = typename ConditionalApply<
    Template, Condition, Otherwise, Ts...
>::type;

template <
    typename T,
    template <typename...> typename Template,
    typename Otherwise
>
using ApplyIfNotVoidT = typename ConditionalApply<
    Template, !std::is_same_v<T, void>, Otherwise, T
>::type;
}
