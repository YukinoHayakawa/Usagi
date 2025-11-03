#pragma once

#include <optional>

#include <Usagi/Runtime/Memory/Backends/MemoryMappingErrorCodes.hpp>

namespace usagi::runtime::memory
{
/*
 * Shio:
 * Provides detailed, platform-independent information about the result of a
 * memory mapping operation. It contains a high-level error code and optional,
 * specific reason codes from different domains.
 */
struct MemoryMappingResult
{
    MemoryMappingErrorCodes code;
    std::optional<platform::FileErrorCode> file_error;
    std::optional<platform::MemoryErrorCode> memory_error;
    std::optional<platform::SecurityErrorCode> security_error;
};
} // namespace usagi::runtime::memory
