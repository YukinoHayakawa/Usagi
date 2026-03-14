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
// todo: using && here forces a rvalue. we need a lvalue or ref
auto protected_invoke(Callable &callable)
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
} // namespace usagi::runtime
