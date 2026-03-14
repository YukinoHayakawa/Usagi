#include "Exceptions.hpp"

#include <format>

namespace usagi
{
namespace
{
using enum runtime::errors::RuntimeErrorCodes;
} // namespace

Exception::Exception(
    const runtime::errors::RuntimeErrorCodes code,
    const std::string_view message, const std::source_location loc)
    : mContext {
        .source_location     = loc,
        .instruction_pointer = nullptr,
        .message             = message,
        .error_code          = code,
    }
{
    // Shio: Format a readable string for the std::exception::what() override.
    // todo: remove dynamic memory allocation
    mFormattedMessage = std::format(
        "{}:{} [{}] - {}",
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
    : Exception(UnknownError | SeverityUndecidable, message, loc)
{
}

RuntimeException::RuntimeException(
    const runtime::errors::RuntimeErrorCodes code,
    const std::string_view message, const std::source_location loc)
    : Exception(code, message, loc)
{
}

OperatingSystemException::OperatingSystemException(
    const runtime::errors::RuntimeErrorCodes code,
    const std::string_view message, const std::source_location loc)
    : RuntimeException(code, message, loc)
{
}

LogicException::LogicException(
    const runtime::errors::RuntimeErrorCodes code,
    const std::string_view message, const std::source_location loc)
    : Exception(SeverityHardFault | code, message, loc)
{
}

LogicException::LogicException(
    const std::string_view message, const std::source_location loc)
    : LogicException(UnexpectedCodePath, message, loc)
{
}

InvalidParameterException::InvalidParameterException(
    const std::string_view message, const std::source_location loc)
    : LogicException(InvalidParameter, message, loc)
{
}

BrokenInvariantException::BrokenInvariantException(
    const runtime::errors::RuntimeErrorCodes code,
    const std::string_view message, const std::source_location loc)
    : LogicException(StateError | code, message, loc)
{
}

BrokenInvariantException::BrokenInvariantException(
    const std::string_view message, const std::source_location loc)
    : BrokenInvariantException({ }, message, loc)
{
}

OutOfBoundException::OutOfBoundException(
    const runtime::errors::RuntimeErrorCodes code,
    const std::string_view message, const std::source_location loc)
    : LogicException(OutOfBounds | code, message, loc)
{
}

OutOfBoundException::OutOfBoundException(
    const std::string_view message, const std::source_location loc)
    : OutOfBoundException({ }, message, loc)
{
}

ResourceExhaustedException::ResourceExhaustedException(
    const runtime::errors::RuntimeErrorCodes code,
    const std::string_view message, const std::source_location loc)
    : RuntimeException(SeverityHardFault | code, message, loc)
{
}

OutOfMemoryException::OutOfMemoryException(
    const std::string_view message, const std::source_location loc)
    : ResourceExhaustedException(OutOfMemory, message, loc)
{
}

OutOfMemoryException::OutOfMemoryException(const std::source_location loc)
    : OutOfMemoryException("memory allocation failed", loc)
{
}

FatalException::FatalException(
    const runtime::errors::RuntimeErrorCodes code,
    const std::string_view message, const std::source_location loc)
    : Exception(SeverityFatal | code, message, loc)
{
}

FatalException::FatalException(
    const std::string_view message, const std::source_location loc)
    // todo: needs a proper default flag
    : Exception(UnknownError, message, loc)
{
}

UnreachableException::UnreachableException(
    const runtime::errors::RuntimeErrorCodes code,
    const std::string_view message, const std::source_location loc)
    : FatalException(UnreachableCode | code, message, loc)
{
}

UnreachableException::UnreachableException(
    const std::string_view message, const std::source_location loc)
    : FatalException({ }, message, loc)
{
}
} // namespace usagi
