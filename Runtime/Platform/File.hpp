#pragma once

#include <cstdint>
#include <string_view>
#include <filesystem>

#include <Usagi/Runtime/Memory/View.hpp>

namespace usagi::platform::file
{
// File not found.
class ExceptionFileNotFound final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

// Permission denied.
class ExceptionFileAccessDenied final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

// File currently used by another program.
class ExceptionFileBusy final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

#ifdef _WIN32
using NativeFileHandle = void *;

struct NativeFileMappingObject
{
    void *section_handle = nullptr;
    // Original base address allocated by the operating system
    void *base_address = nullptr;
    // Original offset requested by the user
    std::uint64_t offset = 0;
    unsigned long protect = 0;
};
#define USAGI_INVALID_FILE_HANDLE \
    ((::usagi::platform::file::NativeFileHandle)(-1))
#else
using NativeFileHandle = int;
using NativeFileMappingObject = int;
#endif

enum FileOpenMode
{
    OPEN_READ                   = 1 << 0,
    OPEN_WRITE                  = 1 << 1,
};
//
// enum FileShareMode
// {
//     SHARE_READ                  = 1 << 0,
//     SHARE_WRITE                 = 1 << 1,
//     SHARE_DELETE                = 1 << 2,
// };

enum FileOpenOptions
{
    OPTION_CREATE_IF_MISSING    = 1 << 0,
    OPTION_ALWAYS_CREATE_NEW    = 1 << 1,
};

/**
 * \brief Create native file handle.
 * \param path
 * \param mode
 * \param options
 * \return A native file handle pointing to path.
 */
NativeFileHandle open(
    const std::filesystem::path &path,
    FileOpenMode mode,
    FileOpenOptions options);

// Same as open(), but platform-specific error codes are translated into
// exceptions defined above.
NativeFileHandle open_exception_translated(
    const std::filesystem::path &path,
    FileOpenMode mode,
    FileOpenOptions options);

/**
 * \brief Close native file handle.
 * \param file
 */
void close(NativeFileHandle file);

/**
 * \brief
 * \param file
 * \return Current file size in bytes.
 */
std::size_t size(NativeFileHandle file);
std::uint64_t id(NativeFileHandle file);
std::uint64_t last_modification_time(NativeFileHandle file);

/*
 * Atomically replace a file with another file and optionally backup the
 * original file.
 */
void replace_file(
    const std::filesystem::path &replaced_file,
    const std::filesystem::path &replacement_file,
    bool backup,
    const std::filesystem::path &backup_name);

// ========================== Memory Mapping ================================ //

enum MemoryMappingMode
{
    MAPPING_READ    = 1 << 0,
    MAPPING_WRITE   = 1 << 1,
};

struct MemoryMapping
{
    MemoryView heap;
    // This contains internal bookkeeping information of native API.
    // DO NOT MODIFY ITS CONTENT.
    NativeFileMappingObject internal;
};

/**
 * \brief Creates a memory mapping to `file`. The file handle is not needed
 * after the mapping is created. If `offset + length > size(file)`, the file
 * size may be extended.
 * \param file If `file == INVALID_FILE_HANDLE`, the mapping will be backed by
 * page file. The effect would be reserving a range of virtual memory.
 * \param mode
 * \param offset
 * \param length
 * \param initial_commit Initial commit size.
 * \return
 */
MemoryMapping map(
    NativeFileHandle file,
    MemoryMappingMode mode,
    std::uint64_t offset,
    std::uint64_t length,
    std::uint64_t initial_commit);

/**
 * \brief Extend the size of the memory mapping. The old base address may
 * be invalidated. The operation does not involve copying any memory content.
 * If the mapping was created with an offset, the extended mapping will also
 * be based on that offset.
 * \param mapping
 * \param new_size
 */
void remap(MemoryMapping &mapping, std::uint64_t new_size);

/**
 * \brief Destroy the file mapping. Further access to the virtual addresses
 * would cause access violation.
 * \param mapping
 */
void unmap(MemoryMapping &mapping);
}
