#include "MemoryMappedFile.hpp"

#include <cassert>

#include <Usagi/Runtime/Platform/Memory.hpp>

namespace usagi
{
MemoryMappedFile::MemoryMappedFile(
    RegularFile file,
    const platform::file::MemoryMappingMode mode,
    const std::size_t max_size)
    : mFile(std::move(file))
    , mMode(mode)
{
    mMapping = platform::file::map(mFile.handle(), mode, max_size);
}

MemoryMappedFile::~MemoryMappedFile()
{
    // todo: flush entire range?
}

void MemoryMappedFile::prefetch(
    const std::size_t offset,
    const std::size_t size)
{
    assert(offset <= mMapping.heap.length);
    assert(offset + size <= mMapping.heap.length);

    // todo what if the memory has been offered out before?

    platform::memory::prefetch(base_byte_view() + offset, size);
}

void MemoryMappedFile::release(const std::size_t offset, const std::size_t size)
{
    assert(offset <= mMapping.heap.length);
    assert(offset + size <= mMapping.heap.length);

    assert(reinterpret_cast<std::size_t>(base_byte_view() + offset)
        % platform::memory::page_size() == 0);
    assert(size
        % platform::memory::page_size() == 0);
    assert(size > 0);

    platform::memory::offer(base_byte_view() + offset, size);
}

void MemoryMappedFile::flush(const std::size_t offset, const std::size_t size)
{
    assert(offset <= mMapping.heap.length);
    assert(offset + size <= mMapping.heap.length);

    assert(mMode & platform::file::MAPPING_WRITE);

    platform::memory::flush(base_byte_view() + offset, size);
}
}
