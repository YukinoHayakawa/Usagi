#pragma once

#include <Usagi/Runtime/Platform/File.hpp>

#include "RegularFile.hpp"

namespace usagi
{
// Memory-mapped files. The mapped region can be immediately used after mapping.
// However, hint functions can be used to explicitly manage physical memory
// usage. Writing past the current end of the file extends the file size. The
// content between the old end and the new end is undefined and dependent on
// the operating system.
class MemoryMappedFile
{
    RegularFile mFile;
    platform::file::MemoryMapping mMapping;
    platform::file::MemoryMappingMode mMode;

public:
    MemoryMappedFile() = default;
    MemoryMappedFile(
        RegularFile file,
        platform::file::MemoryMappingMode mode,
        std::size_t max_size);
    ~MemoryMappedFile();

    MemoryMappedFile(const MemoryMappedFile &other) = delete;
    MemoryMappedFile(MemoryMappedFile &&other) noexcept = default;
    MemoryMappedFile & operator=(const MemoryMappedFile &other) = delete;
    MemoryMappedFile & operator=(MemoryMappedFile &&other) noexcept = default;

    RegularFile & file() { return mFile; }
    std::size_t max_size() const noexcept { return mMapping.heap.length; }

    void * base_view()
    {
        return mMapping.heap.base_address;
    }

    char * base_byte_view()
    {
        return static_cast<char*>(mMapping.heap.base_address);
    }

    // Prefetch the specified range into memory in a non-blocking way so that
    // subsequent accesses to the range could be potentially efficient.
    void prefetch(std::size_t offset, std::size_t size);

    // Declare that the specified range is no longer needed and can be released.
    // The pages may not be immediately recycled depending on the operating
    // system.
    // The range must be page-aligned. Otherwise it may cause access violations
    // to neighbor pages.
    void release(std::size_t offset, std::size_t size);

    // Synchronize the specified range to disk.
    // If the range is not page-aligned, all spanning pages will be flushed.
    void flush(std::size_t offset, std::size_t size);
};
}
