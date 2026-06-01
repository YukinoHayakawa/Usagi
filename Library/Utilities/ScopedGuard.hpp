#pragma once

#include <concepts>
#include <optional>

#include <Usagi/Compilation/Errors.hpp>

namespace usagi::utils
{
/*
 * Executes inbound and outbound rules utilizing RAII.
 *
 * Example:
 * `auto make_guard() { return ScopedGuard([]{}, []{}); )` // won't execute
 * `{ ScopedGuard g { make_guard() }; }` // activated
 *
 * Contract:
 * - Either BOTH of the funcs are called, or none of them. (ctor & dtor
 *   semantics)
 */
template <std::invocable In, std::invocable Out, bool Activated>
struct ScopedGuard
{
    /*
     * Copies deleted for obvious reasons cuz this guard is executed in local
     * scope.
     */
    ScopedGuard(const ScopedGuard &other)                = delete;
    ScopedGuard &operator=(const ScopedGuard &other)     = delete;
    /*
     * A guard must never be moved. The only valid chain is to construct a
     * deactivated guard and move it into an activated one.
     */
    ScopedGuard(ScopedGuard &&other)                     = delete;
    /*
     * Same reason. And checking for `*this` is much a hassle.
     */
    ScopedGuard &operator=(ScopedGuard &&other) noexcept = delete;

    /*
     * Construct a guard but it won't execute. (not activated)
     * Usually used by functions like `make_xxx_guard()`.
     */
    constexpr ScopedGuard(In &&inbound_func, Out &&outbound_func)
        requires (!Activated)
        : inbound_func(std::move(inbound_func))
        , outbound_func(std::move(outbound_func))
    {
    }

    /*
     * Receives a guard and executes the inbound func.
     */
    constexpr explicit ScopedGuard(ScopedGuard<In, Out, false> &&other) noexcept
        requires (Activated)
        : inbound_func { std::move(other.inbound_func) }
        , outbound_func { std::move(other.outbound_func) }
    {
        (void)std::move(other); // suppress warning.
        if(inbound_func) (*inbound_func)();
    }

    constexpr ~ScopedGuard()
    {
        if constexpr(!Activated) return;

        if(outbound_func)
        {
            (*outbound_func)();
        }
        else
        {
            raise_compile_error("broken invariant: no cleanup function.");
        }
    }

private:
    std::optional<In>  inbound_func;
    std::optional<Out> outbound_func;

    // To allow the converting ctor to work.
    friend struct ScopedGuard<In, Out, !Activated>;

    // Make sure ourself is not a callable.
    static_assert(!std::invocable<ScopedGuard>);
};

template <std::invocable In, std::invocable Out>
ScopedGuard(In &&in, Out &&out) -> ScopedGuard<In, Out, false>;

/*
 * This guide is necessary because overload candidates are considered *before*
 * the actual invocation is made. Therefore, both the deleted move ctor and the
 * converting move ctor will be *considered*, causing ambiguity.
 */
template <std::invocable In, std::invocable Out>
ScopedGuard(ScopedGuard<In, Out, false> &&) -> ScopedGuard<In, Out, true>;

namespace static_tests
{
consteval auto test_scoped_guard()
{
    struct s
    {
        int a = false;
        int b = false;

        constexpr auto make_guard()
        {
            return ScopedGuard { [&] { ++a; }, [&] { ++b; } };
        }
    } ctx;

    // initial states
    if(ctx.a != 0) return 1;
    if(ctx.b != 0) return 2;
    {
        // making a guard doesn't activate it
        ScopedGuard g { [&] { ++ctx.a; }, [&] { ++ctx.b; } };
        // nothing should be touched
        if(ctx.a != 0) return 3;
        if(ctx.b != 0) return 4;
    }
    // nothing should be touched
    if(ctx.a != 0) return 5;
    if(ctx.b != 0) return 6;
    {
        // activated by conversion
        ScopedGuard g { ctx.make_guard() };
        // inbound called
        if(ctx.a != 1) return 7;
        if(ctx.b != 0) return 8;
    }
    // both called
    if(ctx.a != 1) return 9;
    if(ctx.b != 1) return 10;

    return 0;
}

static_assert(test_scoped_guard() == 0);
} // namespace static_tests
} // namespace usagi::utils
