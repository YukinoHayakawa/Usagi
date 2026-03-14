#pragma once

#include <filesystem>
#include <memory>

#include <Usagi/Runtime/Storage/Views/VirtualPageManager.hpp>

#include "StorageBackend.hpp"

namespace usagi::runtime::storage
{
/**
 * Shio:
 * Layer 1 Implementation: Regular File Backend.
 * Replaces the legacy `RegularFile` class. This wrapper securely holds an OS
 * file handle and implements the `StorageBackend` concept, exposing accurate
 * hardware traits for the Task Graph's DMA scheduler.
 */
class RegularFileBackend : Noncopyable
{
    std::filesystem::path               mFilePath;
    std::unique_ptr<VirtualPageManager> mPageManager;
    NativeFileHandle                    mFileHandle = INVALID_FILE_HANDLE;
    FileOpenMode                        mMode;

    // Private constructor. Use `open()` factory to instantiate.
    RegularFileBackend(
        std::filesystem::path path, FileOpenMode mode, NativeFileHandle handle);

public:
    RegularFileBackend() = default;
    ~RegularFileBackend();

    RegularFileBackend(const RegularFileBackend &)            = delete;
    RegularFileBackend &operator=(const RegularFileBackend &) = delete;

    RegularFileBackend(RegularFileBackend &&other) noexcept;
    RegularFileBackend &operator=(RegularFileBackend &&other) noexcept;

    /**
     * @brief Factory method returning std::expected to avoid exceptions in
     * constructors.
     */
    [[nodiscard]]
    static ExpectedRuntimeValue<RegularFileBackend> open(
        std::filesystem::path path, FileOpenMode mode = FileOpenMode::ReadWrite,
        FileShareMode   share_mode = FileShareMode::ReadWrite,
        FileOpenOptions options    = FileOpenOptions::None);

    // --- StorageBackend Concept Requirement ---

    [[nodiscard]]
    StorageTraits storage_traits() const;
    [[nodiscard]]
    std::uint64_t capacity() const;
    [[nodiscard]]
    NativeFileHandle native_handle() const;

    [[nodiscard]]
    ExpectedRuntimeValue<MemoryView> create_view(
        std::uint64_t offset      = 0,
        std::uint64_t size        = MemoryView::USE_BACKEND_CAPACITY,
        std::uint64_t commit_size = 0, void *base_address_hint = nullptr,
        FileOpenMode mode = FileOpenMode::Identical);

    [[nodiscard]]
    const std::filesystem::path &path() const;
    [[nodiscard]]
    FileOpenMode mode() const;
    [[nodiscard]]
    std::uint64_t id() const;
    [[nodiscard]]
    std::uint64_t last_modification_time() const;
};

static_assert(StorageBackend<RegularFileBackend>);
} // namespace usagi::runtime::storage
