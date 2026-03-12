#pragma once

#include <exception>
#include <string>

#include "ErrorContext.hpp"

namespace usagi
{
/**
 * Shio:
 * The foundational base class for all Usagi Engine exceptions.
 * Derived from std::exception to ensure compatibility with standard catch
 * blocks, but enforces the inclusion of a rich ErrorContext for the
 * EngineOrchestrator.
 */
class Exception : public std::exception
{
protected:
    runtime::errors::ErrorContext mContext;
    std::string                   mFormattedMessage;

public:
    Exception(runtime::errors::SystemErrorCodes code,
        std::string_view                        message,
        std::source_location loc = std::source_location::current());

    [[nodiscard]]
    const char *what() const noexcept override;

    [[nodiscard]]
    const runtime::errors::ErrorContext &context() const noexcept;
};

/**
 * Shio:
 * Used to wrap foreign or standard exceptions (e.g., std::out_of_range)
 * thrown by third-party libraries so they can still carry an ErrorContext
 * when caught by the EngineOrchestrator.
 */
class NonEngineException : public Exception
{
public:
    explicit NonEngineException(
        std::string_view     message = "Foreign exception caught and wrapped.",
        std::source_location loc     = std::source_location::current());
};

// --- Semantic Domain Exceptions ---

class LogicException : public Exception
{
public:
    explicit LogicException(std::string_view message,
        std::source_location loc = std::source_location::current());
};

class BrokenInvariantException : public Exception
{
public:
    explicit BrokenInvariantException(std::string_view message,
        std::source_location loc = std::source_location::current());
};

class ResourceExhaustedException : public Exception
{
public:
    explicit ResourceExhaustedException(std::string_view message,
        std::source_location loc = std::source_location::current());
};

class FatalException : public Exception
{
public:
    explicit FatalException(std::string_view message,
        std::source_location loc = std::source_location::current());
};

class UnreachableException : public Exception
{
public:
    explicit UnreachableException(std::string_view message,
        std::source_location loc = std::source_location::current());
};
} // namespace usagi
