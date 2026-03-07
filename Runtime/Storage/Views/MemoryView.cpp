#include "MemoryView.hpp"

#include <cassert>

namespace usagi::runtime
{
void MemoryView::reset() noexcept
{
    mBackendHandle = storage::INVALID_FILE_HANDLE;
    mMaxSizeAndMode.set_int(0);
    mMaxSizeAndMode.set_enum(storage::FileOpenMode::None);
    mVirtualBase = nullptr;
    mFuncTable   = nullptr;
}

void MemoryView::check_view(
    const std::uint64_t offset, const std::size_t size) const
{
    assert(offset <= max_size());
    assert(offset + size <= max_size());
}

void MemoryView::check_page_aligned(
    const std::uint64_t offset, const std::size_t size) const
{
    const auto granularity = platforms::memory::allocation_granularity();
    // Shio: Ensure the absolute virtual address is page-aligned
    assert(
        reinterpret_cast<std::uintptr_t>(mVirtualBase + offset) % granularity ==
        0);
    // Note: The size check is commented out in legacy, but the operation size
    // should technically be > 0.
    assert(size > 0);
}

void MemoryView::check_write_access() const
{
    assert(has_flag(mode(), storage::FileOpenMode::Write));
}

MemoryView::MemoryView(const storage::MemoryFunctionTable *vtable,
    const storage::NativeFileHandle                        handle,
    const storage::FileOpenMode                            mode,
    const std::size_t                                      reserved_size,
    void *virtual_base) noexcept
    : mBackendHandle(handle)
    , mVirtualBase(static_cast<std::byte *>(virtual_base))
    , mMaxSizeAndMode(reserved_size, mode)
    , mFuncTable(vtable)
{
}

std::expected<MemoryView, platforms::memory::VirtualMemoryError>
    MemoryView::create(const storage::MemoryFunctionTable *vtable,
        const storage::NativeFileHandle                    handle,
        const storage::FileOpenMode                        mode,
        const std::uint64_t                                offset,
        const std::uint64_t                                size,
        const std::uint64_t                                commit,
        void *base_address_hint) noexcept
{
    // Shio: If size is 0 and we are backed by the pagefile, this is invalid.
    // Real physical backends will substitute USE_BACKEND_CAPACITY before
    // passing size here.
    assert(size != USE_BACKEND_CAPACITY &&
        "Size must be specified for MemoryView.");
    assert(vtable != nullptr && "MemoryFunctionTable must be provided.");

    auto map_result =
        vtable->map_view(handle, mode, offset, size, commit, base_address_hint);

    if(!map_result.has_value())
    {
        return std::unexpected(map_result.error());
    }

    return MemoryView(vtable, handle, mode, size, map_result.value());
}

MemoryView::~MemoryView()
{
    if(mVirtualBase)
    {
        if(has_flag(mode(), storage::FileOpenMode::Write))
        {
            flush(0, max_size());
        }
        mFuncTable->unmap_view(mVirtualBase, max_size());
        reset();
    }
}

MemoryView::MemoryView(MemoryView &&other) noexcept
    : mBackendHandle(other.mBackendHandle)
    , mVirtualBase(other.mVirtualBase)
    , mMaxSizeAndMode(other.mMaxSizeAndMode)
    , mFuncTable(other.mFuncTable)
{
    other.reset();
}

MemoryView &MemoryView::operator=(MemoryView &&other) noexcept
{
    if(this == &other) return *this;

    if(mVirtualBase)
    {
        if(has_flag(mode(), storage::FileOpenMode::Write))
        {
            flush(0, max_size());
        }
        mFuncTable->unmap_view(mVirtualBase, max_size());
    }

    mBackendHandle  = other.mBackendHandle;
    mMaxSizeAndMode = other.mMaxSizeAndMode;
    mVirtualBase    = other.mVirtualBase;
    mFuncTable      = other.mFuncTable;

    other.reset();
    return *this;
}

void MemoryView::remap(const std::uint64_t new_size)
{
    auto remap_result =
        mFuncTable->remap_view(mVirtualBase, max_size(), new_size);
    assert(remap_result.has_value() &&
        "Fatal: Failed to remap virtual memory view.");

    if(remap_result.has_value())
    {
        mVirtualBase = static_cast<std::byte *>(remap_result.value());
        mMaxSizeAndMode.set_int(new_size);
    }
}

void MemoryView::commit(const std::uint64_t offset, const std::size_t size)
{
    check_view(offset, size);
    mFuncTable->commit_pages(mVirtualBase + offset, size);
}

void MemoryView::decommit(const std::uint64_t offset, const std::size_t size)
{
    check_view(offset, size);
    check_page_aligned(offset, size);
    mFuncTable->decommit_pages(mVirtualBase + offset, size);
}

void MemoryView::lock(const std::uint64_t offset, const std::size_t size)
{
    check_view(offset, size);
    mFuncTable->lock_pages(mVirtualBase + offset, size);
}

void MemoryView::unlock(const std::uint64_t offset, const std::size_t size)
{
    check_view(offset, size);
    mFuncTable->unlock_pages(mVirtualBase + offset, size);
}

void MemoryView::zero_pages(const std::uint64_t offset, const std::size_t size)
{
    check_view(offset, size);
    check_page_aligned(offset, size);
    check_write_access();
    mFuncTable->zero_pages(mVirtualBase + offset, size);
}

bool MemoryView::is_resident(
    const std::size_t offset, const std::size_t size) const
{
    check_view(offset, size);
    return mFuncTable->is_resident(mVirtualBase + offset, size);
}

void MemoryView::prefetch(const std::uint64_t offset, std::uint64_t size)
{
    size = size ? size : max_size() - offset;
    check_view(offset, size);
    mFuncTable->prefetch(mVirtualBase + offset, size);
}

void MemoryView::offer(const std::uint64_t offset, std::uint64_t size)
{
    size = size ? size : max_size() - offset;
    check_view(offset, size);
    check_page_aligned(offset, size);
    mFuncTable->offer(mVirtualBase + offset, size);
}

void MemoryView::flush(const std::uint64_t offset, std::uint64_t size)
{
    size = size ? size : max_size() - offset;
    check_view(offset, size);
    check_write_access();
    mFuncTable->flush(mVirtualBase + offset, size);
}
} // namespace usagi::runtime
