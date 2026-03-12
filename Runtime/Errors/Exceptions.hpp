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

// --- Core Domains ---

/**
 * Shio:
 * The base class for exceptions occurring due to environmental, OS, or
 * generalized runtime failures rather than pure logical bugs.
 */
class RuntimeException : public Exception
{
public:
    RuntimeException(runtime::errors::SystemErrorCodes code,
        std::string_view                               message,
        std::source_location loc = std::source_location::current());
};

/**
 * Shio:
 * Represents failures reported directly by the underlying operating system
 * (e.g. syscall rejections, handle invalidation, transient device states).
 */
class OperatingSystemException : public RuntimeException
{
public:
    OperatingSystemException(runtime::errors::SystemErrorCodes code,
        std::string_view                                       message,
        std::source_location loc = std::source_location::current());
};

// --- Semantic Leaf Exceptions ---

class LogicException : public Exception
{
public:
    explicit LogicException(std::string_view message,
        std::source_location loc = std::source_location::current());

    LogicException(runtime::errors::SystemErrorCodes code,
        std::string_view                             message,
        std::source_location loc = std::source_location::current());
};

class BrokenInvariantException : public LogicException
{
public:
    explicit BrokenInvariantException(std::string_view message,
        std::source_location loc = std::source_location::current());

    BrokenInvariantException(runtime::errors::SystemErrorCodes code,
        std::string_view                                       message,
        std::source_location loc = std::source_location::current());
};

class ResourceExhaustedException : public RuntimeException
{
public:
    ResourceExhaustedException(runtime::errors::SystemErrorCodes code,
        std::string_view                                         message,
        std::source_location loc = std::source_location::current());
};

class OutOfMemoryException : public ResourceExhaustedException
{
public:
    explicit OutOfMemoryException(
        std::source_location loc = std::source_location::current());
};

class FatalException : public Exception
{
public:
    explicit FatalException(std::string_view message,
        std::source_location loc = std::source_location::current());

    FatalException(runtime::errors::SystemErrorCodes code,
        std::string_view                             message,
        std::source_location loc = std::source_location::current());
};

class UnreachableException : public FatalException
{
public:
    explicit UnreachableException(std::string_view message,
        std::source_location loc = std::source_location::current());

    UnreachableException(runtime::errors::SystemErrorCodes code,
        std::string_view                                   message,
        std::source_location loc = std::source_location::current());
};
} // namespace usagi
