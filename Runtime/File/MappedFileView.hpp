#pragma once

#include <utility>

#include <Usagi/Runtime/Platform/File.hpp>

namespace usagi
{
class RegularFile;

// Memory-mapped files. The mapped region can be immediately used after mapping.
// Hint functions can be used to explicitly manage physical memory usage.
class MappedFileView
{
    platform::file::MemoryMapping mMapping;
    platform::file::MemoryMappingMode mMode;

    void reset();

    void check_view(std::uint64_t offset, std::size_t size);
    // check for page alignment if operation causes future access violation
    void check_page_aligned(std::uint64_t offset, std::size_t size);
    void check_write_access();

public:
    /**
     * \brief Create a file mapping. If `file` is `nullptr`, the mapping will
     * be backed by page file.
     * \param file
     * \param mode
     * \param offset
     * \param size
     * \param commit
     */
    MappedFileView(
        RegularFile *file,
        platform::file::MemoryMappingMode mode,
        std::uint64_t offset,
        std::uint64_t size,
        std::uint64_t commit);
    ~MappedFileView();

    MappedFileView(const MappedFileView &other) = delete;

    MappedFileView(MappedFileView &&other) noexcept
        : mMapping { std::move(other.mMapping) }
        , mMode { other.mMode }
    {
    }

    MappedFileView & operator=(const MappedFileView &other) = delete;

    MappedFileView & operator=(MappedFileView &&other) noexcept
    {
        if(this == &other)
            return *this;
        mMapping = std::move(other.mMapping);
        mMode = other.mMode;
        return *this;
    }

    std::size_t max_size() const noexcept { return mMapping.heap.length; }

    void * base_view()
    {
        return mMapping.heap.base_address;
    }

    char * base_view_byte()
    {
        return static_cast<char *>(mMapping.heap.base_address);
    }

    void remap(std::uint64_t new_size);

    // todo lock/unlock

    /*
    // Allocate physical pages for specified virtual address range.
    void commit(std::uint64_t offset, std::size_t size);

    // Free physical pages for specified virtual address range.
    void decommit(std::uint64_t offset, std::size_t size);
    */

    // Prefetch the specified range into memory in a non-blocking way so that
    // subsequent accesses to the range could be potentially efficient.
    void prefetch(std::uint64_t offset, std::uint64_t size);

    // Declare that the specified range is no longer needed and can be released.
    // The pages may not be immediately recycled depending on the operating
    // system.
    // The range must be page-aligned. Otherwise it may cause later access
    // violations to neighbor pages.
    void offer(std::uint64_t offset, std::uint64_t size);

    // Synchronize the specified range to disk.
    // If the range is not page-aligned, all spanning pages will be flushed.
    void flush(std::uint64_t offset = 0, std::uint64_t size = 0);
};
}
