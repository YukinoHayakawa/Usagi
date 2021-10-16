#pragma once

#include <utility>
#include <string_view>

#include <Usagi/Library/Memory/Noncopyable.hpp>
#include <Usagi/Runtime/Platform/File.hpp>

namespace usagi
{
class RegularFile;

// Memory-mapped files. The mapped region can be immediately used after mapping.
// Hint functions can be used to explicitly manage physical memory usage.
class MappedFileView : Noncopyable
{
    platform::file::MemoryMapping mMapping;
    platform::file::MemoryMappingMode mMode;

    void reset();

    void check_view(std::uint64_t offset, std::size_t size);
    // check for page alignment if operation causes future access violation
    void check_page_aligned(std::uint64_t offset, std::size_t size);
    void check_write_access();

public:
    constexpr static std::uint64_t USE_FILE_SIZE = 0;

    MappedFileView() = default;

    /**
     * \brief Create a file mapping. If `file` is `nullptr`, the mapping will
     * be backed by page file.
     * \param file
     * \param mode
     * \param offset
     * \param size If 0, the original file size will be used.
     * \param commit
     */
    MappedFileView(
        RegularFile *file,
        platform::file::MemoryMappingMode mode,
        std::uint64_t offset = 0,
        std::uint64_t size = USE_FILE_SIZE,
        std::uint64_t commit = 0);
    ~MappedFileView();

    MappedFileView(const MappedFileView &other) = delete;

    MappedFileView(MappedFileView &&other) noexcept
        : mMapping { std::move(other.mMapping) }
        , mMode { other.mMode }
    {
        other.reset();
    }

    MappedFileView & operator=(const MappedFileView &other) = delete;

    MappedFileView & operator=(MappedFileView &&other) noexcept
    {
        if(this == &other)
            return *this;
        mMapping = std::move(other.mMapping);
        mMode = other.mMode;
        other.reset();
        return *this;
    }

    auto mode() const{ return mMode; }
    std::size_t max_size() const noexcept { return mMapping.heap.length; }

    MemoryRegion memory_region() const
    {
        return mMapping.heap;
    }

    void * base_view() const
    {
        return mMapping.heap.base_address;
    }

    char * base_byte_view() const
    {
        return static_cast<char *>(mMapping.heap.base_address);
    }

    std::string_view base_string_view() const
    {
        return std::string_view(base_byte_view(), max_size());
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
    // If `size == 0` the operation is performed on [offset, max_size - offset).
    void prefetch(std::uint64_t offset = 0, std::uint64_t size = 0);

    // Declare that the specified range is no longer needed and can be released.
    // The pages may not be immediately recycled depending on the operating
    // system.
    // The range must be page-aligned. Otherwise it may cause later access
    // violations to neighbor pages.
    // If `size == 0` the operation is performed on [offset, max_size - offset).
    void offer(std::uint64_t offset = 0, std::uint64_t size = 0);

    // Synchronize the specified range to disk.
    // If the range is not page-aligned, all spanning pages will be flushed.
    // If `size == 0` the operation is performed on [offset, max_size - offset).
    void flush(std::uint64_t offset = 0, std::uint64_t size = 0);
};
}
