#pragma once

#include <type_traits>
#include <utility>

#include <Usagi/Library/Memory/Nonmovable.hpp>

namespace usagi
{
template <typename Callable>
    requires std::is_invocable_v<Callable>
struct ScopeExitGuard : Nonmovable
{
    Callable mCallableCopy;

    ScopeExitGuard() = default;

    ScopeExitGuard(Callable CallableCopy)
        : mCallableCopy { std::move(CallableCopy) }
    {
    }

    ~ScopeExitGuard() { mCallableCopy(); }
};

decltype(auto) MakeScopeExitGuard(auto && Callable)
{
    return ScopeExitGuard { std::forward<decltype(Callable)>(Callable) };
}
} // namespace usagi
