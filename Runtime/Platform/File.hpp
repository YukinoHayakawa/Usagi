#pragma once

#include <string>

#include <Usagi/Runtime/Memory/Region.hpp>

namespace usagi::platform::file
{
#ifdef _WIN32
using NativeFileHandle = void *;
using NativeFileMappingObject = void *;
#else
using NativeFileHandle = int;
using NativeFileMappingObject = int;
#endif

enum FileOpenMode
{
    OPEN_READ                   = 1 << 0,
    OPEN_WRITE                  = 1 << 1,
};

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
    std::u8string_view path,
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

/**
 * \brief Read bytes into the designated buffer.
 * \param file
 * \param offset
 * \param buf
 * \param size
 * \return Number of bytes read.
 */
std::size_t read_at(
    NativeFileHandle file,
    std::size_t offset,
    void *buf,
    std::size_t size);

/**
 * \brief Write bytes into the file.
 * \param file
 * \param offset
 * \param buf
 * \param size Number of bytes written.
 * \return
 */
std::size_t write_at(
    NativeFileHandle file,
    std::size_t offset,
    const void *buf,
    std::size_t size);

// ========================== Memory Mapping ================================ //

enum MemoryMappingMode
{
    MAPPING_READ    = 1 << 0,
    MAPPING_WRITE   = 1 << 1,
};

struct MemoryMapping
{
    MemoryRegion heap;
    NativeFileMappingObject mapping;
};

// Create memory mapping for file.
// Returns the actual affected memory region.
MemoryMapping map(
    NativeFileHandle file,
    MemoryMappingMode mode,
    std::size_t max_size);

// Removes memory mapped file.
// Returns the actual affected memory region.
void unmap(const MemoryMapping &mapping);
}
