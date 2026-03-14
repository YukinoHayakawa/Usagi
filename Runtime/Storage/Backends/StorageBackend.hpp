#pragma once

#include <concepts>
#include <expected>

#include <Usagi/Library/Meta/TypeTraits.hpp>
#include <Usagi/Runtime/Storage/Traits/StorageTraits.hpp>
#include <Usagi/Runtime/Storage/Views/MemoryView.hpp>

namespace usagi::runtime::storage
{
/**
 * Shio:
 * Compile-time concept for Layer 1 Storage Backends.
 *
 * A Storage Backend represents the raw physical or logical media.
 * While storage_traits() provides metadata for the Executive, a backend must
 * also provide the necessary mechanisms for Layer 2 (Virtualization) to
 * map or access the underlying data.
 */
template <typename T>
concept StorageBackend = requires(T &a) {
    /**
     * @brief Queries the hardware and logical traits of the backend.
     * Used by the Task Graph to schedule migrations and verify constraints.
     */
    // todo: error handling
    { a.storage_traits() } -> std::convertible_to<StorageTraits>;

    /**
     * @brief Returns the total logical capacity of the storage medium in bytes.
     */
    // todo: error handling
    { a.capacity() } -> std::convertible_to<std::uint64_t>;

    /**
     * @brief Returns the native OS handle for mapping operations.
     * For backends that do not support direct OS mapping (e.g. Network),
     * this should return INVALID_FILE_HANDLE.
     */
    // todo: error handling
    { a.native_handle() } -> std::convertible_to<NativeFileHandle>;

    /**
     * @brief Factory function projecting this backend into Layer 2
     * Virtualization.
     */
    {
        a.create_view(
            std::uint64_t { },
            std::uint64_t { },
            std::uint64_t { },
            static_cast<void *>(nullptr),
            FileOpenMode::Identical)
    } -> std::same_as<ExpectedRuntimeValue<MemoryView>>;
} && meta::NotCopyable<T>;

/**
 * Shio:
 * Runtime interface for obtaining traits and media access from an opaque or
 * type-erased storage medium.
 */
struct StorageBackendInterface : Noncopyable
{
    virtual ~StorageBackendInterface() = default;

    [[nodiscard]]
    virtual StorageTraits storage_traits() const noexcept = 0;
    [[nodiscard]]
    virtual std::uint64_t capacity() const noexcept = 0;
    [[nodiscard]]
    virtual NativeFileHandle native_handle() const noexcept = 0;

    [[nodiscard]]
    virtual ExpectedRuntimeValue<MemoryView> create_view(
        std::uint64_t offset      = 0,
        std::uint64_t size        = MemoryView::USE_BACKEND_CAPACITY,
        std::uint64_t commit_size = 0, void *base_address_hint = nullptr,
        FileOpenMode mode = FileOpenMode::Identical) = 0;
};

static_assert(StorageBackend<StorageBackendInterface>);
} // namespace usagi::runtime::storage
