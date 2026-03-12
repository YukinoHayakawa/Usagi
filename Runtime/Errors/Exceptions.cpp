#include "Exceptions.hpp"

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
    // Shio: Format a readable string for the std::exception::what()
    // override. In a full implementation, we might use std::format here.
    mFormattedMessage = std::string(loc.file_name()) + ":" +
        std::to_string(loc.line()) + " [" + std::string(loc.function_name()) +
        "] - " + std::string(message);
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

LogicException::LogicException(
    const std::string_view message, const std::source_location loc)
    : Exception(runtime::errors::SystemErrorCodes::InvalidParameter |
              runtime::errors::SystemErrorCodes::SeverityLogicError,
          message,
          loc)
{
}

BrokenInvariantException::BrokenInvariantException(
    const std::string_view message, const std::source_location loc)
    : Exception(runtime::errors::SystemErrorCodes::StateError |
              runtime::errors::SystemErrorCodes::SeverityHardFault,
          message,
          loc)
{
}

ResourceExhaustedException::ResourceExhaustedException(
    const std::string_view message, const std::source_location loc)
    : Exception(runtime::errors::SystemErrorCodes::OutOfMemory |
              runtime::errors::SystemErrorCodes::SeverityHardFault,
          message,
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

UnreachableException::UnreachableException(
    const std::string_view message, const std::source_location loc)
    : Exception(runtime::errors::SystemErrorCodes::StateError |
              runtime::errors::SystemErrorCodes::SeverityFatal,
          message,
          loc)
{
}
} // namespace usagi
