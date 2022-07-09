#pragma once

#include <utility>
#include <type_traits>

namespace usagi
{
template <typename ErrorCodeT, typename ValueT>
class ReturnValue
{
protected:
    ErrorCodeT mCode;
    // use std::optional to wrap the value when necessary as in usual cases.
    ValueT mValue;

public:
    ReturnValue(ErrorCodeT code, ValueT value = { })
        : mCode(std::move(code))
        , mValue(std::move(value))
    {
    }

    // How to add C++ structured binding support to your own types
    // https://devblogs.microsoft.com/oldnewthing/20201015-00/?p=104369
    template <std::size_t Index>
    // ReSharper disable once CppNotAllPathsReturnValue
    // (intended SFINAE)
    std::tuple_element_t<Index, ReturnValue> & get()
    {
        if constexpr (Index == 0) return mCode;
        if constexpr (Index == 1) return mValue;
    }

    ErrorCodeT code() const { return mCode; }
    const ValueT & value() { return mValue; }
};
}

template <typename ErrorCodeT, typename ValueT>
struct std::tuple_size<usagi::ReturnValue<ErrorCodeT, ValueT>>
    : integral_constant<size_t, 2> {};

template <typename ErrorCodeT, typename ValueT>
struct std::tuple_element<0, usagi::ReturnValue<ErrorCodeT, ValueT>>
{
    using type = ErrorCodeT;
};

template <typename ErrorCodeT, typename ValueT>
struct std::tuple_element<1, usagi::ReturnValue<ErrorCodeT, ValueT>>
{
    using type = ValueT;
};
