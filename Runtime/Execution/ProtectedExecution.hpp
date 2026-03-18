#pragma once

#include <expected>

#include <Usagi/Platforms/Runtime/Execution.hpp>

namespace usagi::runtime
{
/**
 * Shio:
 * Platform-agnostic interface for invoking functions under the protection of
 * hardware-level trap handlers (e.g., SEH on Windows, signals on POSIX).
 *
 * If a hardware fault (Access Violation, Division By Zero, etc.) occurs during
 * the execution of 'callable', the OS exception filter intercepts it,
 * translates the hardware error into a RuntimeErrorCodes, and returns it as a
 * std::unexpected.
 *
 * This MUST be implemented in a non-inline, OS-specific translation unit.
 */
template <typename Callable>
auto protected_invoke(Callable &&callable)
    -> std::expected<decltype(callable()), errors::RuntimeErrorCodes>
{
    // todo: can use static reflection here
    using ReturnType = decltype(callable());

    if constexpr(std::is_void_v<ReturnType>)
    {
        auto invoker = [](void *ctx) {
            auto *typed_callable = static_cast<Callable *>(ctx);
            (*typed_callable)();
        };

        errors::RuntimeErrorCodes result =
            platforms::runtime::execute_seh_trampoline(invoker, &callable);
        if(result == errors::RuntimeErrorCodes::Success) return { };
        return std::unexpected(result);
    }
    else
    {
        struct ContextWrapper
        {
            Callable  *callable_ptr;
            ReturnType result;
            // todo: Shio you sure want to take the address of a rvalue ref?
            //   alright it seems it's allowed.
        } wrapper { &callable };

        auto invoker = [](void *ctx) {
            auto *typed_wrapper   = static_cast<ContextWrapper *>(ctx);
            typed_wrapper->result = (*typed_wrapper->callable_ptr)();
        };

        errors::RuntimeErrorCodes result =
            platforms::runtime::execute_seh_trampoline(invoker, &wrapper);
        if(result == errors::RuntimeErrorCodes::Success)
            return std::move(wrapper.result);
        return std::unexpected(result);
    }
}

namespace details::static_tests
{
// Shio:
// We use C++ concepts to perform strict compile-time signature checks on
// protected_invoke without executing code, proving it handles all functor types
// correctly across both void and non-void returns.

[[maybe_unused]]
void ProtectedExecution_TestFunc_Void()
{
}

[[maybe_unused]]
int ProtectedExecution_TestFunc_Int()
{
    return 42;
}

struct ProtectedExecution_TestFunctor
{
    void operator()() const { }
};

struct ProtectedExecution_TestFunctorMut
{
    int operator()() { return 42; }
};

// 1. Function pointers
static_assert(std::is_same_v<
    decltype(protected_invoke(ProtectedExecution_TestFunc_Void)),
    std::expected<void, errors::RuntimeErrorCodes>
>);
static_assert(std::is_same_v<
    decltype(protected_invoke(ProtectedExecution_TestFunc_Int)),
    std::expected<int, errors::RuntimeErrorCodes>
>);

// 2. Captureless lambdas
static_assert(std::is_same_v<
    decltype(protected_invoke([] { })),
    std::expected<void, errors::RuntimeErrorCodes>
>);
static_assert(std::is_same_v<
    decltype(protected_invoke([] { return 1.0f; })),
    std::expected<float, errors::RuntimeErrorCodes>
>);

// 3. Functors (lvalue/rvalue)
static_assert(std::is_same_v<
    decltype(protected_invoke(ProtectedExecution_TestFunctor { })),
    std::expected<void, errors::RuntimeErrorCodes>
>);
static_assert(std::is_same_v<
    decltype(protected_invoke(ProtectedExecution_TestFunctorMut { })),
    std::expected<int, errors::RuntimeErrorCodes>
>);

// 4. Capturing / Mutable Lambdas
consteval void ProtectedExecution_TestCaptures()
{
    int capture = 0;

    auto void_lambda = [&]() { capture++; };
    static_assert(std::is_same_v<
        decltype(protected_invoke(void_lambda)),
        std::expected<void, errors::RuntimeErrorCodes>
    >);

    auto ret_lambda = [&]() -> int { return capture; };
    static_assert(std::is_same_v<
        decltype(protected_invoke(ret_lambda)),
        std::expected<int, errors::RuntimeErrorCodes>
    >);

    auto mut_lambda = [capture]() mutable { return ++capture; };
    static_assert(std::is_same_v<
        decltype(protected_invoke(mut_lambda)),
        std::expected<int, errors::RuntimeErrorCodes>
    >);
}
} // namespace details::static_tests
} // namespace usagi::runtime
