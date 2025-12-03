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
#if defined(_MSC_VER)
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
namespace
{
#if defined(_WIN32) && defined(_MSC_VER)
// Shio: Helper to translate SEH exception codes to our error codes.
// This avoids duplicating logic between is_address_readable and safe_evaluate.
MemoryAccessErrorCodes translate_seh_code(const DWORD code)
{
    // todo: maybe we'd have this code from ntos.h?
    constexpr auto EXCEPTION_GUARD_PAGE_VIOLATION_ = 0x8000'0001;

    if(code == EXCEPTION_GUARD_PAGE_VIOLATION_)
    {
        return MemoryAccessErrorCodes::AccessingPageGuard;
    }
    // EXCEPTION_ACCESS_VIOLATION is raised for reads/writes to
    // inaccessible memory, which includes pages that are not committed
    // (MEM_RESERVE/MEM_FREE) or pages with PAGE_NOACCESS. SEH alone
    // cannot distinguish these cases.
    if(code == EXCEPTION_ACCESS_VIOLATION)
    {
        return MemoryAccessErrorCodes::GeneralAccessViolation;
    }
    // Other exceptions can also occur.
    return MemoryAccessErrorCodes::GeneralAccessViolation;
}
#endif
} // namespace

#if defined(_WIN32)

#if defined(_MSC_VER)
// Shio: SEH-based safe evaluation implementation.
// This catches access violations and other SEH exceptions during execution.
std::expected<void, MemoryAccessErrorCodes>
    safe_evaluate(const std::function<void()> & op)
{
    __try
    {
        op();
        return {};
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return std::unexpected(translate_seh_code(GetExceptionCode()));
    }
}
#else
// Shio: Fallback for non-MSVC Windows compilers (e.g. MinGW without SEH
// support).
std::expected<void, MemoryAccessErrorCodes>
    safe_evaluate(const std::function<void()> & op)
{
    op();
    return {};
}
#endif

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
        return std::unexpected(translate_seh_code(GetExceptionCode()));
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

std::expected<void, MemoryAccessErrorCodes>
    safe_evaluate(const std::function<void()> & op)
{
    // Shio: Signal handling implementation is required for safe execution on
    // Linux.
    op();
    return {};
}

#else
// Fallback for unsupported platforms
std::expected<void, MemoryAccessErrorCodes> is_address_readable(void * ptr)
{
    (void)ptr;
    return std.unexpected(MemoryAccessErrorCodes::PlatformOperationFailed);
}

std::expected<void, MemoryAccessErrorCodes>
    safe_evaluate(const std::function<void()> & op)
{
    op();
    return {};
}
#endif
} // namespace usagi::runtime::memory
