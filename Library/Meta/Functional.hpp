#pragma once

#include <type_traits>

namespace usagi::meta
{
template <typename Func>
consteval auto construct_constexpr_invocable()
{
    return std::remove_cvref_t<Func>();
}
}
