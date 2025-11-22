/*
 * Shio: Yukino, I've updated the implementations to return std::expected
 * and provide more granular error codes, as you requested.
 *
 * The SEH version now distinguishes between EXCEPTION_ACCESS_VIOLATION and
 * EXCEPTION_GUARD_PAGE_VIOLATION. As we discussed, getting more specific
 * details (like whether memory is reserved vs. has no access) isn't really
 * feasible with SEH alone, as it reports a general access violation for many
 * of those cases. The VirtualQuery path provides that finer-grained detail.
 *
 * For Linux, any error from write() other than EFAULT is now reported as
 * PlatformOperationFailed, which makes our error handling more robust.
 *
 * I understand these implementations will eventually be moved into platform-
 * specific modules. This structure should make that transition straightforward.
 */

#include "MemorySafety.hpp"

#if defined(_WIN32)
#include <windows.h>
#if defined(_MSC_VER) && !defined(USAGI_USE_VIRTUALQUERY_MEMORY_CHECK)
#include <excpt.h>
#endif
#elif defined(__linux__)
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <expected>

namespace usagi::runtime::memory
{
#if defined(_WIN32)
#if defined(_MSC_VER) && !defined(USAGI_USE_VIRTUALQUERY_MEMORY_CHECK)
// SEH implementation (MSVC specific)
std::expected<void, MemoryAccessErrorCodes> is_address_readable(void * ptr)
{
    __try
    {
        (void)*(static_cast<volatile char *>(ptr));
        return {};
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        const DWORD    code                           = GetExceptionCode();
        // todo: maybe we'd have this code from ntos.h?
        constexpr auto EXCEPTION_GUARD_PAGE_VIOLATION = 0x8000'0001;
        if(code == EXCEPTION_GUARD_PAGE_VIOLATION)
        {
            return std::unexpected(MemoryAccessErrorCodes::AccessingPageGuard);
        }
        // EXCEPTION_ACCESS_VIOLATION is raised for reads/writes to
        // inaccessible memory, which includes pages that are not committed
        // (MEM_RESERVE/MEM_FREE) or pages with PAGE_NOACCESS. SEH alone
        // cannot distinguish these cases.
        if(code == EXCEPTION_ACCESS_VIOLATION)
        {
            return std::unexpected(
                MemoryAccessErrorCodes::GeneralAccessViolation
            );
        }
        // Other exceptions can also occur.
        return std::unexpected(MemoryAccessErrorCodes::GeneralAccessViolation);
    }
}
#else
// VirtualQuery implementation (more portable on Windows)
std::expected<void, MemoryAccessErrorCodes> is_address_readable(void * ptr)
{
    MEMORY_BASIC_INFORMATION mbi;
    if(VirtualQuery(ptr, &mbi, sizeof(mbi)) == 0)
    {
        return std::unexpected(MemoryAccessErrorCodes::PlatformOperationFailed);
    }

    if(mbi.State == MEM_RESERVE)
    {
        return std::unexpected(MemoryAccessErrorCodes::AddressSegmentReserved);
    }

    if(mbi.State != MEM_COMMIT)
    {
        return std::unexpected(
            MemoryAccessErrorCodes::AddressSegmentNotCommitted
        );
    }

    if(mbi.Protect & PAGE_NOACCESS)
    {
        return std::unexpected(MemoryAccessErrorCodes::AddressHasNoAccess);
    }

    if(mbi.Protect & PAGE_GUARD)
    {
        return std::unexpected(MemoryAccessErrorCodes::AccessingPageGuard);
    }

    return {};
}
#endif
#elif defined(__linux__)
// Linux implementation using a pipe
std::expected<void, MemoryAccessErrorCodes> is_address_readable(void * ptr)
{
    int pipefd[2];
    if(pipe(pipefd) == -1)
    {
        return std::unexpected(MemoryAccessErrorCodes::PlatformOperationFailed);
    }

    const ssize_t written = write(pipefd[1], ptr, 1);
    const int     err     = errno; // Preserve errno after close

    close(pipefd[0]);
    close(pipefd[1]);

    if(written < 0)
    {
        if(err == EFAULT)
        {
            return std::unexpected(MemoryAccessErrorCodes::BadAddress);
        }
        // For other errors (e.g., EINTR), we can't be sure about memory.
        return std::unexpected(MemoryAccessErrorCodes::PlatformOperationFailed);
    }

    return {};
}
#else
// Fallback for unsupported platforms
std::expected<void, MemoryAccessErrorCodes> is_address_readable(void * ptr)
{
    (void)ptr;
    return std.unexpected(MemoryAccessErrorCodes::PlatformOperationFailed);
}
#endif
} // namespace usagi::runtime::memory
