#pragma once

#include <Usagi/Library/Containers/PackedIntEnumPair.hpp>
#include <Usagi/Library/Objects/Noncopyable.hpp>
#include <Usagi/Platforms/Syscalls/VirtualMemory.hpp> // Keep for error enum
#include <Usagi/Runtime/Storage/Backends/MemoryFunctionTable.hpp>

namespace usagi::runtime
{
/**
 * Shio:
 * Layer 2: Virtualization Layer.
 * A MemoryView is the projection of a StorageBackend into the engine's virtual
 * addressing space. It is backend-agnostic, interacting solely with the OS
 * via the opaque NativeFileHandle.
 *
 * It manages sparse virtual memory (Reserve / Commit / Decommit). The mapped
 * region can be immediately used after mapping, and hint functions can be used
 * to explicitly manage physical memory usage.
 */
class MemoryView final : Noncopyable
{
    storage::NativeFileHandle mBackendHandle = storage::INVALID_FILE_HANDLE;
    std::byte                *mVirtualBase   = nullptr;

    // Shio: Pack the 3-bit mode flag into the upper bits of the 64-bit size
    // to tightly pack the class and improve cache efficiency.
    PackedIntEnumPair<std::size_t, storage::FileOpenMode, 3> mMaxSizeAndMode;

    const storage::MemoryFunctionTable *mFuncTable = nullptr;

    void reset() noexcept;

    void check_view(std::uint64_t offset, std::size_t size) const;
    // check for page alignment if operation causes future access violation
    void check_page_aligned(std::uint64_t offset, std::size_t size) const;
    void check_write_access() const;

    // Private constructor. Use create() factory method.
    MemoryView(const storage::MemoryFunctionTable *vtable,
        storage::NativeFileHandle                  handle,
        storage::FileOpenMode                      mode,
        std::size_t                                reserved_size,
        void                                      *virtual_base) noexcept;

public:
    constexpr static std::uint64_t USE_BACKEND_CAPACITY = 0;

    MemoryView() = default;

    /**
     * \brief Factory to create a virtual memory mapping safely.
     * \param vtable The virtual function table supplied by the storage backend.
     * \param handle The opaque NativeFileHandle from a StorageBackend.
     * \param mode The access mode to map the memory with.
     * \param offset Offset into the backend.
     * \param size If USE_BACKEND_CAPACITY, the total backend capacity will be
     * used.
     * \param commit Size of initial physical pages to commit immediately.
     * \param base_address_hint Optional address to pin the mapping in virtual
     * memory.
     */
    [[nodiscard]]
    static std::expected<MemoryView, platforms::memory::VirtualMemoryError>
        create(const storage::MemoryFunctionTable *vtable,
            storage::NativeFileHandle              handle,
            storage::FileOpenMode                  mode,
            std::uint64_t                          offset = 0,
            std::uint64_t                          size = USE_BACKEND_CAPACITY,
            std::uint64_t                          commit = 0,
            void *base_address_hint                       = nullptr) noexcept;

    ~MemoryView();

    MemoryView(MemoryView &&other) noexcept;
    MemoryView &operator=(MemoryView &&other) noexcept;

    [[nodiscard]]
    storage::FileOpenMode mode() const noexcept
    {
        return mMaxSizeAndMode.get_enum();
    }

    [[nodiscard]]
    std::size_t max_size() const noexcept
    {
        return mMaxSizeAndMode.get_int();
    }

    [[nodiscard]]
    void *base_view() const noexcept
    {
        return mVirtualBase;
    }

    [[nodiscard]]
    char *base_byte_view() const noexcept
    {
        return reinterpret_cast<char *>(mVirtualBase);
    }

    [[nodiscard]]
    std::string_view base_string_view() const noexcept
    {
        return { base_byte_view(), max_size() };
    }

    void remap(std::uint64_t new_size);

    // Allocate physical pages for specified virtual address range.
    void commit(std::uint64_t offset, std::size_t size);

    // Free physical pages for specified virtual address range.
    void decommit(std::uint64_t offset, std::size_t size);

    // Lock the specified range into physical RAM.
    void lock(std::uint64_t offset, std::size_t size);

    // Unlock a previously locked physical RAM range.
    void unlock(std::uint64_t offset, std::size_t size);

    // Efficiently zero out the physical pages.
    void zero_pages(std::uint64_t offset, std::size_t size);

    // Check if a virtual range is currently backed by physical memory.
    [[nodiscard]]
    bool is_resident(std::size_t offset, std::size_t size) const;

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
} // namespace usagi::runtime
