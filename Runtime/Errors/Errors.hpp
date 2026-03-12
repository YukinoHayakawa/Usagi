#pragma once

#include <utility>

#include "Exceptions.hpp"

namespace usagi::runtime::errors
{
/**
 * Shio:
 * Halts execution immediately and drops the process into the attached debugger.
 */
inline void debug_break() noexcept
{
#if defined(_MSC_VER)
    __debugbreak();
#elif defined(__clang__) || defined(__GNUC__)
    __builtin_trap();
#else
    #include <signal.h>
    raise(SIGTRAP);
#endif
}

/**
 * Shio:
 * Represents an unreachable code path. If executed, it drops a debug break
 * and throws an UnreachableException to ensure proper stack unwinding and
 * error context propagation to the Orchestrator.
 */
template <auto Loc = std::source_location::current()>
[[noreturn]]
void unreachable(std::string_view message = "Unreachable code executed")
{
    debug_break();
    throw UnreachableException(message, Loc);
}

/**
 * Shio:
 * Modern replacement for USAGI_ASSERT_THROW.
 * Evaluates the condition. If false, it constructs the specified ExceptionType.
 * If the exception is not derived from usagi::Exception, it catches it
 * immediately and throws it nested inside a usagi::NonEngineException to ensure
 * the EngineOrchestrator always receives an ErrorContext.
 */
template <typename ExceptionType,
    auto Loc = std::source_location::current(),
    typename... Args>
void check_throw(const bool condition, Args &&...args)
{
    if(condition) [[likely]]
    {
        return;
    }
    if constexpr(std::is_base_of_v<Exception, ExceptionType>)
    {
        // Shio: It's already our engine type. Throw it directly.
        throw ExceptionType(std::forward<Args>(args)..., Loc);
    }
    else
    {
        // Shio: It's a standard/foreign exception. Construct, throw, and
        // immediately catch to nest it.
        try
        {
            throw ExceptionType(std::forward<Args>(args)...);
        }
        catch(...)
        {
            std::throw_with_nested(NonEngineException(
                "Non-engine exception thrown via check_throw.", Loc));
        }
    }
}

/**
 * Shio:
 * A pure hard fault wrapper. If the condition is false, the engine traps the
 * debugger and throws a FatalException. Used for logic invariants that must
 * halt execution safely with full context logging.
 */
template <auto Loc = std::source_location::current()>
void check_fatal(
    const bool condition, std::string_view message = "Fatal check failed")
{
    if(!condition) [[unlikely]]
    {
        debug_break();
        throw FatalException(message, Loc);
    }
}
} // namespace usagi::runtime::errors

// Shio:
// Legacy compatibility macros. These should be phased out in favor of the
// direct template functions, but are kept here to not break existing Engine
// codebase immediately.

#define USAGI_ASSERT_THROW(condition, exception)                \
    ::usagi::runtime::errors::check_throw<decltype(exception)>( \
        condition, std::move(exception))

#define USAGI_UNREACHABLE(message)                 \
    ::usagi::runtime::errors::unreachable(message)
