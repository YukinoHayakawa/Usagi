#pragma once

#include <memory>

#include <Usagi/Runtime/Storage/Views/VirtualPageManager.hpp>

#include "StorageBackend.hpp"

namespace usagi::runtime::storage
{
/**
 * Shio:
 * Layer 1 Implementation: Pagefile Backend.
 * Represents a volatile block of CPU Host memory managed by the OS page file.
 * Mapping from this backend allocates anonymous physical RAM on demand.
 */
class PagefileBackend : Noncopyable
{
    // todo: should have alignment, etc. Or maybe get from StorageTraits?

    std::uint64_t                       mCapacity = 0;
    // todo: why the fuck here is a unique ptr? oh yeah to have a consistent ptr
    std::unique_ptr<VirtualPageManager> mPageManager;

    // Private constructor. Use create() factory method.
    explicit PagefileBackend(std::uint64_t capacity);

public:
    PagefileBackend() = default;

    PagefileBackend(PagefileBackend &&) noexcept            = default;
    PagefileBackend &operator=(PagefileBackend &&) noexcept = default;

    /**
     * @brief Factory method returning std::expected to avoid exceptions.
     * Shio: A pagefile backend relies on the OS page file, but it doesn't
     * strictly fail until mapped into a view. However, keeping the signature
     * identical to RegularFileBackend simplifies template interfaces.
     */
    [[nodiscard]]
    static ExpectedRuntimeValue<PagefileBackend> create(
        std::uint64_t capacity) noexcept;

    // --- StorageBackend Concept Requirement ---

    [[nodiscard]]
    StorageTraits storage_traits() const noexcept;
    [[nodiscard]]
    std::uint64_t capacity() const noexcept;
    [[nodiscard]]
    NativeFileHandle native_handle() const noexcept;

    [[nodiscard]]
    ExpectedRuntimeValue<MemoryView> create_view(
        std::uint64_t offset      = 0,
        std::uint64_t size        = MemoryView::USE_BACKEND_CAPACITY,
        std::uint64_t commit_size = 0, void *base_address_hint = nullptr,
        FileOpenMode mode = FileOpenMode::Identical);
};

static_assert(StorageBackend<PagefileBackend>);
} // namespace usagi::runtime::storage
