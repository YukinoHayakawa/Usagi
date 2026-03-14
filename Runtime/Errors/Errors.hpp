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
[[noreturn]]
inline void unreachable(
    const std::string_view     message = "Unreachable code executed",
    const std::source_location loc     = std::source_location::current())
{
    debug_break();
    throw UnreachableException(message, loc);
}

/**
 * Shio:
 * Modern replacement for USAGI_ASSERT_THROW.
 * Evaluates the condition. If false, it constructs the specified ExceptionType.
 * If the exception is not derived from usagi::Exception, it catches it
 * immediately and throws it nested inside a usagi::NonEngineException to ensure
 * the EngineOrchestrator always receives an ErrorContext.
 */
template <typename ExceptionType, typename... Args>
void check_throw(const bool condition, std::source_location loc, Args &&...args)
{
    if(condition) [[likely]]
    {
        return;
    }
    if constexpr(std::is_base_of_v<Exception, ExceptionType>)
    {
        // Shio: It's already our engine type. Throw it directly.
        throw ExceptionType(std::forward<Args>(args)..., loc);
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
                "Non-engine exception thrown via check_throw.", loc));
        }
    }
}

/**
 * Shio:
 * A pure hard fault wrapper. If the condition is false, the engine traps the
 * debugger and throws a FatalException. Used for logic invariants that must
 * halt execution safely with full context logging.
 */
inline void check_fatal(
    const bool condition, const std::string_view message,
    const std::source_location loc = std::source_location::current())
{
    if(!condition) [[unlikely]]
    {
        debug_break();
        throw FatalException(message, loc);
    }
}
} // namespace usagi::runtime::errors

#define USAGI_CHECK_THROW(exception_type, condition, ...)          \
    ::usagi::runtime::errors::check_throw<exception_type>(         \
        (condition), std::source_location::current(), __VA_ARGS__)

#define USAGI_CHECK_FATAL(condition, message)                    \
    ::usagi::runtime::errors::check_fatal(                       \
        (condition), (message), std::source_location::current())
