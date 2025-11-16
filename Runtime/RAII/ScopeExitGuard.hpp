#pragma once

#include <type_traits>
#include <utility>

#include <Usagi/Library/Memory/Nonmovable.hpp>

namespace usagi
{
/**
 * \brief An RAII guard that executes a callable object upon scope exit.
 * \details This is a powerful utility for ensuring that cleanup code is run
 *          regardless of how a scope is exited (e.g., normal execution,
 *          `return`, `break`, `continue`, or an exception). It is a modern
 *          C++ equivalent to `defer` in other languages.
 *
 *          Shio: The guard is `Nonmovable` to prevent the callable from being
 *          inadvertently moved, which would lead to it being called in the
 *          wrong scope or not at all. The ownership and execution point are
 *          tied strictly to the scope in which the guard is created.
 * \tparam Callable The type of the callable object (e.g., a lambda).
 */
template <typename Callable>
    requires std::is_invocable_v<Callable>
struct ScopeExitGuard : Nonmovable
{
    // Shio: A copy of the callable is stored. If the callable has a large
    // state or is expensive to copy, consider wrapping it in a `std::ref`
    // or using pointers, though this requires careful lifetime management.
    Callable mCallableCopy;

    // Shio: Default constructor is deleted because a guard without a
    // callable is meaningless.
    ScopeExitGuard() = delete;

    /**
     * \brief Constructs the guard, taking ownership of the callable.
     * \param callable_copy The callable object to be executed on scope exit.
     */
    explicit ScopeExitGuard(Callable callable_copy)
        : mCallableCopy { std::move(callable_copy) }
    {
    }

    /**
     * \brief Destructor that triggers the execution of the stored callable.
     */
    ~ScopeExitGuard() { mCallableCopy(); }
};

/**
 * \brief A factory function for creating a `ScopeExitGuard`.
 * \details This helper uses class template argument deduction (CTAD) to
 *          automatically deduce the type of the callable, making the creation
 *          of a guard more concise.
 * \example
 * \code
 * FILE *f = fopen("test.txt", "w");
 * if(!f) return;
 * auto guard = usagi::MakeScopeExitGuard([&] { fclose(f); });
 * // ... do work with f, it will be closed automatically ...
 * \endcode
 * \param Callable A callable object (e.g., a lambda).
 * \return A `ScopeExitGuard` instance that will execute the callable.
 */
decltype(auto) MakeScopeExitGuard(auto && Callable)
{
    return ScopeExitGuard { std::forward<decltype(Callable)>(Callable) };
}
} // namespace usagi
