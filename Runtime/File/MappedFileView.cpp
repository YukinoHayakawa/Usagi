#include "MappedFileView.hpp"

#include <cassert>
#include <stdexcept>

#include <Usagi/Runtime/ErrorHandling.hpp>
#include <Usagi/Runtime/Platform/Memory.hpp>

#include "RegularFile.hpp"

namespace usagi
{
void MappedFileView::reset()
{
    mMapping.heap.base_address = nullptr;
}

void MappedFileView::check_view(std::uint64_t offset, std::size_t size)
{
    assert(offset <= mMapping.heap.length);
    assert(offset + size <= mMapping.heap.length);
}

void MappedFileView::check_page_aligned(std::uint64_t offset, std::size_t size)
{
    assert(
        reinterpret_cast<std::size_t>(base_byte_view() + offset)
        % platform::memory::page_size() == 0
    );
    // assert(size % platform::memory::page_size() == 0);
    assert(size > 0);
}

void MappedFileView::check_write_access()
{
    assert(mMode & platform::file::MAPPING_WRITE);
}

MappedFileView::MappedFileView(
    RegularFile *file,
    platform::file::MemoryMappingMode mode,
    std::uint64_t offset,
    std::uint64_t size,
    std::uint64_t commit)
    : mMode(mode)
{
    using namespace platform::file;

    if(!file && size == USE_FILE_SIZE)
        USAGI_THROW(std::invalid_argument(
            "The size of a pagefile-backed mapping must be specified.")
        );

    // since we have a handle to the file, it must exist.
    if(size == USE_FILE_SIZE)
        size = file->size() - offset;

    // check_page_aligned(offset, size);

    mMapping = map(
        file ? file->handle() : USAGI_INVALID_FILE_HANDLE,
        mode,
        offset,
        size,
        commit
    );
}

MappedFileView::~MappedFileView()
{
    using namespace platform::file;

    if(mMapping.heap.base_address)
    {
        if(mMode & MAPPING_WRITE)
        {
            flush(0, mMapping.heap.length);
        }
        unmap(mMapping);
        reset();
    }
}

/*
void MappedFileView::commit(std::uint64_t offset, std::size_t size)
{
    check_view(offset, size);

    platform::memory::commit(base_view_byte() + offset, size);
}

void MappedFileView::decommit(std::uint64_t offset, std::size_t size)
{
    check_view(offset, size);
    check_page_aligned(offset, size);

    platform::memory::decommit(base_view_byte() + offset, size);
}
*/

void MappedFileView::remap(std::uint64_t new_size)
{
    // check_page_aligned(0, new_size);

    platform::file::remap(mMapping, new_size);
}

void MappedFileView::prefetch(
    const std::uint64_t offset,
    const std::uint64_t size)
{
    check_view(offset, size);

    platform::memory::prefetch(base_byte_view() + offset, size);
}

void MappedFileView::offer(
    const std::uint64_t offset,
    const std::uint64_t size)
{
    check_view(offset, size);
    check_page_aligned(offset, size);

    platform::memory::offer(base_byte_view() + offset, size);
}

void MappedFileView::flush(const std::uint64_t offset, std::uint64_t size)
{
    size = size ? size : max_size();

    check_view(offset, size);
    check_write_access();

    platform::memory::flush(base_byte_view() + offset, size);
}
}
