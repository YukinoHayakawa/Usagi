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

template <typename T>
struct ExtractFirstTemplateArgument;

template <
    template <typename...> typename T,
    typename First,
    typename... Args
>
struct ExtractFirstTemplateArgument<T<First, Args...>>
{
    using type = First;
};

template <typename T>
struct ExtractSecondTemplateArgument;

template <
    template <typename...> typename T,
    typename First,
    typename Second,
    typename... Args
>
struct ExtractSecondTemplateArgument<T<First, Second, Args...>>
{
    using type = Second;
};
}


/*
#define USAGI_TARG_EXPAND_TO_TYPES(...) __VA_ARGS__
#define USAGI_TARG_EXPAND_TO_TYPENAMES(...) __VA_ARGS__

#define USAGI_APPEND_CONSTRAINTS(base_type, member_func, constraints, ...) \
    template <typename... Args, __VA_ARGS__> \
    decltype(auto) member_func(Args &&... args) requires (constraints) \
    { \
        return base_type::member_func<__VA_ARGS__>( \
            std::forward<Args>(args)... \
        ); \
    } \
/*#1#*/