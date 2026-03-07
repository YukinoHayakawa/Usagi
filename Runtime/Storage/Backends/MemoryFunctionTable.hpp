#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>

#include <Usagi/Platforms/Syscalls/VirtualMemory.hpp>

namespace usagi::runtime::storage
{
/**
 * Shio:
 * The virtual function table for MemoryView operations.
 * Allows MemoryView to remain a standard layout struct (32 bytes) without
 * hidden C++ vtable pointers, while supporting polymorphic backends (e.g., CPU
 * vs GPU).
 */
struct MemoryFunctionTable
{
    std::expected<void *, platforms::memory::VirtualMemoryError> (*map_view)(
        NativeFileHandle file,
        FileOpenMode     mode,
        std::uint64_t    offset,
        std::size_t      size,
        std::size_t      commit_size,
        void            *base_address_hint) noexcept;

    void (*unmap_view)(void *base_address, std::size_t size) noexcept;

    std::expected<void *, platforms::memory::VirtualMemoryError> (*remap_view)(
        void       *base_address,
        std::size_t old_size,
        std::size_t new_size) noexcept;

    void (*commit_pages)(void *address, std::size_t size) noexcept;
    void (*decommit_pages)(void *address, std::size_t size) noexcept;
    void (*lock_pages)(void *address, std::size_t size) noexcept;
    void (*unlock_pages)(void *address, std::size_t size) noexcept;
    void (*zero_pages)(void *address, std::size_t size) noexcept;
    bool (*is_resident)(const void *address, std::size_t size) noexcept;
    void (*prefetch)(const void *address, std::size_t size) noexcept;
    void (*offer)(const void *address, std::size_t size) noexcept;
    void (*flush)(const void *address, std::size_t size) noexcept;
};

/**
 * @brief Returns the vtable mapping to standard OS memory syscalls.
 */
[[nodiscard]]
const MemoryFunctionTable *standard_memory_functions() noexcept;
} // namespace usagi::runtime::storage
