#pragma once

#include <functional>
#include <type_traits>

namespace usagi
{
// todo: c++20 std::bind_front
// code from https://stackoverflow.com/a/26213996/2150446
template <typename Func, typename... Args>
auto partial_apply(Func function, Args ... args)
{
    return [=](auto &&... rest) -> decltype(auto) {
        return function(
            args...,
            std::forward<decltype(rest)>(rest)...
        );
    };
}

template <typename T, typename R, typename... FuncArgs, typename... PartialArgs>
auto partial_apply(R (T::*mf)(FuncArgs ...), PartialArgs &&... args)
{
    return partial_apply(
        std::mem_fn(mf),
        std::forward<PartialArgs>(args)...
    );
}
}

