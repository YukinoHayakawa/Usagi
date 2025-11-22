/*
 * Shio: As you requested, Yukino, I've implemented the memory accessibility
 * checks.
 *
 * For Windows, I'm providing two versions. The SEH-based one is faster but
 * relies on MSVC extensions. The VirtualQuery-based one is more portable across
 * compilers on Windows, like Clang. By default, I'm enabling the SEH version
 * when compiling with MSVC for performance, but you can override this by
 * defining USAGI_USE_VIRTUALQUERY_MEMORY_CHECK.
 *
 * For Linux, I'm using the pipe trick, which is a standard POSIX-compliant
 * way to safely check for read access to a memory page.
 *
 * I'll add this new file to the Usagi project in the solution.
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

namespace usagi::runtime::memory
{
#if defined(_WIN32)
#if defined(_MSC_VER) && !defined(USAGI_USE_VIRTUALQUERY_MEMORY_CHECK)
// SEH implementation (MSVC specific)
bool is_address_readable(void * ptr)
{
    __try
    {
        // Volatile is important to prevent the compiler from optimizing away
        // the read.
        (void)*(static_cast<volatile char *>(ptr));
        return true;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // The exception can be EXCEPTION_ACCESS_VIOLATION, but other exceptions
        // can also occur, for example with guard pages.
        // Capturing all exceptions is safer here.
        return false;
    }
}
#else
// VirtualQuery implementation (more portable on Windows)
bool is_address_readable(void * ptr)
{
    MEMORY_BASIC_INFORMATION mbi;
    if(VirtualQuery(ptr, &mbi, sizeof(mbi)) == 0)
    {
        // This can happen if the address is in a reserved region of memory.
        return false;
    }

    // Check if the memory is committed.
    if(mbi.State != MEM_COMMIT)
    {
        return false;
    }

    // Check for no-access and guard pages.
    if((mbi.Protect & PAGE_NOACCESS) || (mbi.Protect & PAGE_GUARD))
    {
        return false;
    }

    // The page is committed and not a guard page, so it is accessible.
    // Any of READ, WRITE, or EXECUTE access is sufficient to be "readable"
    // in the sense that a dereference won't crash immediately.
    return true;
}
#endif
#elif defined(__linux__)
// Linux implementation using a pipe
bool is_address_readable(void * ptr)
{
    // A pipe is created. Writing to a pipe with an invalid buffer
    // will cause write() to return EFAULT instead of crashing.
    int pipefd[2];
    if(pipe(pipefd) == -1)
    {
        // If we can't create a pipe, we can't determine accessibility.
        // Fallback to a safe assumption.
        return false;
    }

    // Attempt to write one byte from the given pointer to the pipe.
    // The write end of the pipe is used. The data is not actually
    // important, only the return value of write().
    ssize_t written = write(pipefd[1], ptr, 1);

    // Clean up the pipe file descriptors.
    close(pipefd[0]);
    close(pipefd[1]);

    // If write() returned an error, check if it was EFAULT.
    if(written < 0 && errno == EFAULT)
    {
        return false;
    }

    // Otherwise, the address is considered accessible.
    // This includes the case where write() succeeds (written > 0) or
    // is interrupted (written < 0 and errno != EFAULT).
    return true;
}
#else
// Fallback for unsupported platforms
bool is_address_readable(void * ptr)
{
    // On unknown platforms, we can't make a safe determination.
    // Returning true might be dangerous, but returning false might
    // break valid logic. We'll assume true but this should be implemented
    // for each platform.
    (void)ptr; // suppress unused parameter warning
    return true;
}
#endif
} // namespace usagi::runtime::memory
