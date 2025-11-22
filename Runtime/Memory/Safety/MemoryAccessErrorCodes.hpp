#pragma once

namespace usagi::runtime::memory
{
// Yukino: write comments for each entry and add other sensible entries.
enum class MemoryAccessErrorCodes
{
    GeneralAccessViolation,
    AccessingPageGuard,
    AddressSegmentReserved,
    AddressSegmentNotCommitted,
    AddressHasNoAccess,
};
} // namespace usagi::a
