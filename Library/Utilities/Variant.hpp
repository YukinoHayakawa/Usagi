#pragma once

#include <variant>

namespace usagi
{
// https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts>
struct Overloaded : Ts...
{
    explicit Overloaded(Ts &&... ts)
        : Ts(std::move(ts))...
    {
    }

    using Ts::operator()...;
};

template <typename T, typename... Vs>
decltype(auto) visit_element(T &&t, Vs &&... vs)
{
    return std::visit(Overloaded(std::forward<Vs>(vs)...), t);
}
}
