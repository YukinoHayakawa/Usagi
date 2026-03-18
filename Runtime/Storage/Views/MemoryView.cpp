#include "MemoryView.hpp"

#include <Usagi/Library/Utilities/Span.hpp>
#include <Usagi/Runtime/Errors/Errors.hpp>
#include <Usagi/Runtime/Storage/Backends/MemoryFunctionTable.hpp>
#include <Usagi/Runtime/Storage/Views/VirtualPageManager.hpp>

namespace usagi::runtime::storage
{
void MemoryView::reset() noexcept
{
    // Overwrite all members.
    new (this) MemoryView();
}

void MemoryView::check_view(
    const std::uint64_t offset, const std::size_t size) const
{
    // todo: debug only
    USAGI_CHECK_THROW(
        OutOfBoundException,
        offset <= max_size(),
        "MemoryView offset out of bounds");
    USAGI_CHECK_THROW(
        OutOfBoundException,
        offset + size <= max_size(),
        "MemoryView size out of bounds");
}

void MemoryView::check_page_aligned(
    const std::uint64_t offset, const std::size_t size) const
{
    const auto granularity = platforms::memory::allocation_granularity();
    // Shio: Ensure the absolute virtual address is page-aligned
    USAGI_CHECK_THROW(
        LogicException,
        reinterpret_cast<std::uintptr_t>(mVirtualBase + offset) % granularity ==
            0,
        errors::RuntimeErrorCodes::AlignmentError,
        "Virtual address is not page-aligned");
    // Note: The size check is commented out in legacy, but the operation size
    // should technically be > 0.
    USAGI_CHECK_THROW(
        InvalidParameterException,
        size > 0,
        "Operation size must be greater than 0");
}

void MemoryView::check_write_access() const
{
    USAGI_CHECK_THROW(
        LogicException,
        has_any_of(mode(), FileOpenMode::Write),
        "MemoryView does not have write access");
}

MemoryView::MemoryView(
    const MemoryFunctionTable *vtable, const NativeFileHandle handle,
    const FileOpenMode mode, const std::size_t reserved_size,
    void *virtual_base, const StorageTraits &traits,
    VirtualPageManager *page_manager) noexcept
    : mBackendHandle(handle)
    , mVirtualBase(static_cast<std::byte *>(virtual_base))
    , mMaxSizeAndMode(reserved_size, mode)
    , mFuncTable(vtable)
    , mTraits(traits)
    , mPageManager(page_manager)
{
}

ExpectedRuntimeValue<MemoryView> MemoryView::create(
    const MemoryFunctionTable *vtable, const NativeFileHandle handle,
    const FileOpenMode mode, const StorageTraits &traits,
    VirtualPageManager *page_manager, const std::uint64_t offset,
    const std::uint64_t size, const std::uint64_t commit,
    void *base_address_hint) noexcept
{
    // Shio: If size is 0 and we are backed by the pagefile, this is invalid.
    // Real physical backends will substitute USE_BACKEND_CAPACITY before
    // passing size here.
    USAGI_CHECK_THROW(
        InvalidParameterException,
        size != USE_BACKEND_CAPACITY,
        "Size must be specified for MemoryView.");
    USAGI_CHECK_THROW(
        InvalidParameterException,
        vtable != nullptr,
        "MemoryFunctionTable must be provided.");

    auto map_result =
        vtable->map_view(handle, mode, offset, size, commit, base_address_hint);

    if(!map_result.has_value())
    {
        return std::unexpected(map_result.error());
    }

    return MemoryView(
        vtable, handle, mode, size, map_result.value(), traits, page_manager);
}

void MemoryView::maybe_flush_unmap()
{
    if(!is_valid()) return;

    // Shio:
    // We never blindly flush the whole range of the view upon object
    // destruction under any circumstance. Depending on the OS, flushing
    // uncommitted pages can result in an error (e.g.
    // STATUS_NOT_MAPPED_DATA). It is the responsibility of the
    // VirtualPageManager (or higher-level logic) to selectively flush only
    // committed pages if required.
    if(has_any_of(mode(), FileOpenMode::Write))
    {
        // todo: let page manager handle flushing dirty pages?
        flush(0, max_size());
    }
    // todo: maybe this should be done via page manager
    mFuncTable->unmap_view(mVirtualBase, max_size());
}

MemoryView::~MemoryView()
{
    maybe_flush_unmap();
    reset();
}

MemoryView::MemoryView(MemoryView &&other) noexcept
    : mBackendHandle(other.mBackendHandle)
    , mVirtualBase(other.mVirtualBase)
    , mMaxSizeAndMode(other.mMaxSizeAndMode)
    , mFuncTable(other.mFuncTable)
    , mTraits(other.mTraits)
    , mPageManager(other.mPageManager)
{
    other.reset();
}

MemoryView &MemoryView::operator=(MemoryView &&other) noexcept
{
    if(this == &other) return *this;

    maybe_flush_unmap();

    mBackendHandle  = other.mBackendHandle;
    mVirtualBase    = other.mVirtualBase;
    mMaxSizeAndMode = other.mMaxSizeAndMode;
    mFuncTable      = other.mFuncTable;
    mTraits         = other.mTraits;
    mPageManager    = other.mPageManager;

    other.reset();
    return *this;
}

// todo: proper error handling
ExpectedRuntimeValue<void> MemoryView::remap(const std::uint64_t new_size)
{
    const auto remap_result =
        mFuncTable->remap_view(mVirtualBase, max_size(), new_size);

    if(remap_result.has_value())
    {
        mVirtualBase = static_cast<std::byte *>(remap_result.value());
        mMaxSizeAndMode.set_int(new_size);
        return { };
    }
    return std::unexpected(remap_result.error());
}

void MemoryView::commit(
    const std::uint64_t offset, const std::size_t size,
    const CommitStrategy strategy)
{
    check_view(offset, size);
    if(strategy == CommitStrategy::Bypass || !mPageManager)
    {
        const auto result =
            mFuncTable->commit_pages(mVirtualBase + offset, size);
        USAGI_CHECK_THROW(
            OutOfMemoryException,
            result.has_value(),
            "commit_pages failed during Bypass: OOM or system limitation");
    }
    else
    {
        mPageManager->commit(this, offset, size, strategy);
    }
}

void MemoryView::decommit(
    const std::uint64_t offset, const std::size_t size,
    const CommitStrategy strategy)
{
    check_view(offset, size);
    if(strategy == CommitStrategy::Bypass || !mPageManager)
    {
        check_page_aligned(offset, size);
        const auto result =
            mFuncTable->decommit_pages(mVirtualBase + offset, size);
        USAGI_CHECK_THROW(
            OperatingSystemException,
            result.has_value(),
            // todo: get the actual error
            errors::RuntimeErrorCodes::UnknownError,
            "decommit_pages failed during Bypass: OS rejected valid decommit "
            "request");
    }
    else
    {
        mPageManager->decommit(this, offset, size, strategy);
    }
}

void MemoryView::lock(const std::uint64_t offset, const std::size_t size)
{
    check_view(offset, size);
    // todo: notify page manager
    const auto result = mFuncTable->lock_pages(mVirtualBase + offset, size);
    USAGI_CHECK_THROW(
        ResourceExhaustedException,
        result.has_value(),
        // todo: get the actual error
        errors::RuntimeErrorCodes::UnknownError,
        "lock_pages failed: OS physical memory quota reached");
}

void MemoryView::unlock(const std::uint64_t offset, const std::size_t size)
{
    check_view(offset, size);
    // todo: notify page manager
    const auto result = mFuncTable->unlock_pages(mVirtualBase + offset, size);
    USAGI_CHECK_THROW(
        OperatingSystemException,
        result.has_value(),
        // todo: get the actual error
        errors::RuntimeErrorCodes::UnknownError,
        "unlock_pages failed: OS rejected unlock request");
}

void MemoryView::zero_pages(const std::uint64_t offset, const std::size_t size)
{
    check_view(offset, size);
    check_page_aligned(offset, size);
    check_write_access();
    const auto result = mFuncTable->zero_pages(mVirtualBase + offset, size);
    USAGI_CHECK_THROW(
        OperatingSystemException,
        result.has_value(),
        // todo: get the actual error
        errors::RuntimeErrorCodes::UnknownError,
        "zero_pages failed: OS physical layer error");
}

// todo report error
bool MemoryView::is_resident(
    const std::size_t offset, const std::size_t size) const
{
    check_view(offset, size);
    const auto result = mFuncTable->is_resident(mVirtualBase + offset, size);
    return result.value_or(false);
}

// todo report error
void MemoryView::prefetch(const std::uint64_t offset, std::uint64_t size)
{
    size = size ? size : max_size() - offset;
    check_view(offset, size);
    // Intentional discard, prefetch is a hint, though OS might report errors
    mFuncTable->prefetch(mVirtualBase + offset, size);
}

// todo report error
void MemoryView::offer(const std::uint64_t offset, std::uint64_t size)
{
    size = size ? size : max_size() - offset;
    check_view(offset, size);
    check_page_aligned(offset, size);
    // Intentional discard, offer is a hint, though OS might report errors
    mFuncTable->offer(mVirtualBase + offset, size);
}

void MemoryView::flush(const std::uint64_t offset, std::uint64_t size)
{
    size = size ? size : max_size() - offset;
    check_view(offset, size);
    check_write_access();
    // todo: properly handle attempts to flush to pagefile
    if(native_handle() == platforms::memory::pagefile_handle()) return;
    // todo: notify page manager
    const auto result = mFuncTable->flush(mVirtualBase + offset, size);
    USAGI_CHECK_THROW(
        OperatingSystemException,
        result.has_value(),
        // todo: get the actual error
        errors::RuntimeErrorCodes::UnknownError,
        "flush failed: underlying device I/O error");
}

void MemoryView::copy_memory(
    void *dst, const void *src, const std::size_t size) const noexcept
{
    const ByteSpan view_span(mVirtualBase, max_size());
    const ByteSpan dst_span(dst, size);
    const ByteSpan src_span(src, size);

    USAGI_CHECK_FATAL(
        view_span.contains(dst_span), "copy_memory destination out of bounds");
    USAGI_CHECK_FATAL(
        view_span.contains(src_span), "copy_memory source out of bounds");
    USAGI_CHECK_FATAL(
        !dst_span.overlaps_with(src_span),
        "copy_memory regions overlap, use move_memory instead");

    const auto result = mFuncTable->copy_memory(dst, src, size);
    USAGI_CHECK_FATAL(result.has_value(), "copy_memory failed");
}

void MemoryView::move_memory(
    void *dst, const void *src, const std::size_t size) const noexcept
{
    const ByteSpan view_span(mVirtualBase, max_size());
    const ByteSpan dst_span(dst, size);
    const ByteSpan src_span(src, size);

    USAGI_CHECK_FATAL(
        view_span.contains(dst_span), "move_memory destination out of bounds");
    USAGI_CHECK_FATAL(
        view_span.contains(src_span), "move_memory source out of bounds");

    const auto result = mFuncTable->move_memory(dst, src, size);
    USAGI_CHECK_FATAL(result.has_value(), "move_memory failed");
}
} // namespace usagi::runtime::storage
