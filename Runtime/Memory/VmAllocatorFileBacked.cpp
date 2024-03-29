﻿#include "VmAllocatorFileBacked.hpp"

#include <cassert>
#include <stdexcept>

#include <Usagi/Runtime/ErrorHandling.hpp>
#include <Usagi/Runtime/File/RegularFile.hpp>

namespace usagi
{
void VmAllocatorFileBacked::check_positive_size(const std::size_t size)
{
    if(size == 0) USAGI_THROW(std::bad_alloc());
}

void VmAllocatorFileBacked::check_allocated_by_us(void *ptr)
{
    if(ptr != mMapping->base_view()) USAGI_THROW(std::bad_alloc());
}

void VmAllocatorFileBacked::check_mapping_created(bool created) const
{
    if(created != bool(mMapping)) USAGI_THROW(std::bad_alloc());
}

void VmAllocatorFileBacked::check_has_backing_file(bool has) const
{
    if(mFilePath.empty() == has) USAGI_THROW(std::bad_alloc());
}

VmAllocatorFileBacked::VmAllocatorFileBacked(
    VmAllocatorFileBacked &&other) noexcept
    : mFilePath { std::move(other.mFilePath) }
    , mMapping { std::move(other.mMapping) }
{
}

VmAllocatorFileBacked & VmAllocatorFileBacked::operator=(
    VmAllocatorFileBacked &&other) noexcept
{
    if(this == &other)
        return *this;
    mFilePath = std::move(other.mFilePath);
    mMapping = std::move(other.mMapping);
    return *this;
}

void VmAllocatorFileBacked::set_backing_file(std::filesystem::path file)
{
    check_has_backing_file(false);

    mFilePath = std::move(file);
}

void * VmAllocatorFileBacked::allocate(std::size_t size)
{
    check_has_backing_file(true);
    check_mapping_created(false);
    // if size == 0 the allocated size would equal to the current file size.
    // check_positive_size(size);

    using namespace platform::file;

    RegularFile file {
        mFilePath.u8string(),
        FileOpenMode(OPEN_READ | OPEN_WRITE),
        FileOpenOptions(OPTION_CREATE_IF_MISSING)
    };
    // if the file already exist, always use the full file.
    // todo: is this necessary?
    if(file.size() > 0) size = MappedFileView::USE_FILE_SIZE;

    mMapping = file.create_view(0, size, 0);

    return mMapping->base_view();
}

void * VmAllocatorFileBacked::reallocate(void *old_ptr, std::size_t new_size)
{
    if(old_ptr == nullptr)
        return allocate(new_size);

    check_mapping_created(true);
    check_positive_size(new_size);
    check_allocated_by_us(old_ptr);

    mMapping->remap(new_size);

    return mMapping->base_view();
}

void VmAllocatorFileBacked::deallocate(void *ptr)
{
    check_mapping_created(true);
    check_allocated_by_us(ptr);

    mMapping.reset();
}

std::size_t VmAllocatorFileBacked::max_size() const noexcept
{
    if(mFilePath.empty()) return 0;
    return space(mFilePath).available + mMapping->max_size();
}
}
