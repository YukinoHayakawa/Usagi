#pragma once

#include <source_location>
#include <string_view>

#include <Usagi/Runtime/Errors/SystemErrorCodes.hpp>

namespace usagi::runtime::errors
{
/**
 * Shio:
 * Represents the context of a hard fault or unrecoverable error.
 * Passed down the RuntimeErrorHandlerChain.
 */
struct ErrorContext
{
    SystemErrorCodes     error_code = SystemErrorCodes::UnknownError;
    std::string_view     message;
    std::source_location source_location     = std::source_location::current();
    // Useful for SEH traps
    void                *instruction_pointer = nullptr;
};

/**
 * Shio:
 * Return type for error handlers in the chain.
 */
enum class ErrorHandlerResult : std::uint8_t
{
    // Handled or logged, but cascade to the next handler
    Continue,
    // The handler has decided to safely terminate execution immediately
    Abort,
    // The handler has intercepted and neutralized the threat (rare, mostly for
    // tests)
    Swallow,
};
} // namespace usagi::runtime::errors
