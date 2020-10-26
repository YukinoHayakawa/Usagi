#pragma once

#include <filesystem>
#include <optional>

#include <Usagi/Concept/Allocator/ReallocatableAllocator.hpp>
#include <Usagi/Runtime/File/MappedFileView.hpp>

namespace usagi
{
/**
 * \brief Virtual memory allocator backed by disk file. Reallocation of size
 * larger than the file size would extend the file on the disk.
 */
class VmAllocatorFileBacked
{
    std::filesystem::path mFilePath;
    std::optional<MappedFileView> mMapping;

    static void check_positive_size(std::size_t size);
    void check_allocated_by_us(void *ptr);
    void check_mapping_created(bool created) const;
    void check_has_backing_file(bool has) const;

public:
    VmAllocatorFileBacked() = default;
    VmAllocatorFileBacked(VmAllocatorFileBacked &&other) noexcept;
    VmAllocatorFileBacked & operator=(VmAllocatorFileBacked &&other) noexcept;

    void set_backing_file(std::filesystem::path file);
    const std::filesystem::path & path() const { return mFilePath; }

    // Move assignment and copy ctor & assignment implicitly deleted.

    void * allocate(std::size_t size);
    void * reallocate(void *old_ptr, std::size_t new_size);
    void deallocate(void *ptr);

    std::size_t max_size() const noexcept;
};
static_assert(ReallocatableAllocator<VmAllocatorFileBacked>);
}
