#include "Exceptions.hpp"

#include <format>

namespace usagi
{
Exception::Exception(const runtime::errors::SystemErrorCodes code,
    const std::string_view                                   message,
    const std::source_location                               loc)
    : mContext {
        .error_code          = code,
        .message             = message,
        .source_location     = loc,
        .instruction_pointer = nullptr,
    }
{
    // Shio: Format a readable string for the std::exception::what() override.
    mFormattedMessage = std::format("{}:{} [{}] - {}",
        loc.file_name(),
        loc.line(),
        loc.function_name(),
        message);
}

const char *Exception::what() const noexcept
{
    return mFormattedMessage.c_str();
}

const runtime::errors::ErrorContext &Exception::context() const noexcept
{
    return mContext;
}

NonEngineException::NonEngineException(
    const std::string_view message, const std::source_location loc)
    : Exception(runtime::errors::SystemErrorCodes::UnknownError |
              runtime::errors::SystemErrorCodes::SeverityUndecidable,
          message,
          loc)
{
}

RuntimeException::RuntimeException(const runtime::errors::SystemErrorCodes code,
    const std::string_view     message,
    const std::source_location loc)
    : Exception(code, message, loc)
{
}

OperatingSystemException::OperatingSystemException(
    const runtime::errors::SystemErrorCodes code,
    const std::string_view                  message,
    const std::source_location              loc)
    : RuntimeException(code, message, loc)
{
}

LogicException::LogicException(
    const std::string_view message, const std::source_location loc)
    : Exception(runtime::errors::SystemErrorCodes::UnexpectedCodePath |
              runtime::errors::SystemErrorCodes::SeverityLogicError,
          message,
          loc)
{
}

LogicException::LogicException(const runtime::errors::SystemErrorCodes code,
    const std::string_view                                             message,
    const std::source_location                                         loc)
    : Exception(code, message, loc)
{
}

BrokenInvariantException::BrokenInvariantException(
    const std::string_view message, const std::source_location loc)
    : LogicException(runtime::errors::SystemErrorCodes::StateError |
              runtime::errors::SystemErrorCodes::SeverityHardFault,
          message,
          loc)
{
}

BrokenInvariantException::BrokenInvariantException(
    const runtime::errors::SystemErrorCodes code,
    const std::string_view                  message,
    const std::source_location              loc)
    : LogicException(code, message, loc)
{
}

ResourceExhaustedException::ResourceExhaustedException(
    const runtime::errors::SystemErrorCodes code,
    const std::string_view                  message,
    const std::source_location              loc)
    : RuntimeException(
          code | runtime::errors::SystemErrorCodes::SeverityHardFault,
          message,
          loc)
{
}

OutOfMemoryException::OutOfMemoryException(const std::source_location loc)
    : ResourceExhaustedException(runtime::errors::SystemErrorCodes::OutOfMemory,
          "memory allocation failed",
          loc)
{
}

FatalException::FatalException(
    const std::string_view message, const std::source_location loc)
    : Exception(runtime::errors::SystemErrorCodes::UnknownError |
              runtime::errors::SystemErrorCodes::SeverityFatal,
          message,
          loc)
{
}

FatalException::FatalException(const runtime::errors::SystemErrorCodes code,
    const std::string_view                                             message,
    const std::source_location                                         loc)
    : Exception(code, message, loc)
{
}

UnreachableException::UnreachableException(
    const std::string_view message, const std::source_location loc)
    : FatalException(runtime::errors::SystemErrorCodes::UnreachableCode |
              runtime::errors::SystemErrorCodes::SeverityFatal,
          message,
          loc)
{
}

UnreachableException::UnreachableException(
    const runtime::errors::SystemErrorCodes code,
    const std::string_view                  message,
    const std::source_location              loc)
    : FatalException(code, message, loc)
{
}
} // namespace usagi
