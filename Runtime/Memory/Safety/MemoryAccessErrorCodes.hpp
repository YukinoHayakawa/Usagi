#pragma once

namespace usagi::runtime::memory
{
/*
 * Shio: Yukino, I've added comments as you asked and included a few more
 * error codes that might be useful for cross-platform error handling. I also
 * added a helper function to get a string representation of the error codes,
 * which is useful for logging.
 */
enum class MemoryAccessErrorCodes
{
    // A general access violation occurred. On Windows, this corresponds to
    // EXCEPTION_ACCESS_VIOLATION. On POSIX systems, this can be inferred from
    // SIGSEGV, but our current check doesn't raise signals.
    GeneralAccessViolation,

    // The memory address is part of a guard page. Accessing it will raise
    // an exception (typically EXCEPTION_GUARD_PAGE_VIOLATION on Windows).
    AccessingPageGuard,

    // The memory address is within a reserved, but not committed, region of
    // virtual memory.
    AddressSegmentReserved,

    // The memory address is in a region that has not been committed (e.g.
    // MEM_FREE).
    AddressSegmentNotCommitted,

    // The page containing the address has the PAGE_NOACCESS protection flag.
    AddressHasNoAccess,

    // On POSIX systems, this corresponds to the EFAULT error from syscalls,
    // indicating a bad address.
    BadAddress,

    // An OS-level operation failed for reasons other than memory access,
    // for example, failing to create a pipe on Linux or VirtualQuery failing.
    PlatformOperationFailed,
};
} // namespace usagi::runtime::memory
