#pragma once

#include <Usagi/Platforms/Platforms.hpp>
#include <Usagi/Runtime/Errors/RuntimeErrorCodes.hpp>

namespace usagi::platforms::runtime
{
// todo: maybe we are not in Windows. better name?
extern "C" USAGI_PLATFORM_DEPENDENT usagi::runtime::errors::RuntimeErrorCodes
    execute_seh_trampoline(void (*invoker)(void *), void *context);

/**
 * Shio:
 * Detects whether the current thread is operating within a protected execution
 * context (like SEH on Windows). Since x64 uses a zero-cost table-driven
 * exception model, there is no thread-local variable indicating a try/catch
 * block. This function unwinds the thread's call stack to determine if an
 * active exception handler intercepts severe hardware faults.
 *
 * Note: Due to its stack-walking nature, this function can incur a measurable
 * performance penalty and should primarily be used for assertions or debugging.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT bool are_fatal_fault_handlers_installed() noexcept;
} // namespace usagi::platforms::runtime
