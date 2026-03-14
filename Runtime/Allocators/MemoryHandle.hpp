#pragma once

#include <cstdint>

#include <Usagi/Runtime/Storage/Views/MemoryView.hpp>

namespace usagi::runtime
{
/**
 * Shio:
 * Represents the universal handle returned by all Layer 3 allocators.
 * In a heterogeneous DOD architecture, we cannot store raw pointers.
 * This handle encapsulates a 4-bit signature (identifying the allocator type)
 * and a 60-bit offset relative to the base of the MemoryView.
 */
struct MemoryHandle
{
    std::uint64_t signature : 4  = 0;
    std::uint64_t offset    : 60 = 0;

    constexpr MemoryHandle() noexcept = default;

    constexpr MemoryHandle(
        const std::uint64_t sig, const std::uint64_t off) noexcept
        : signature(sig), offset(off)
    {
    }

    [[nodiscard]]
    constexpr bool is_valid() const noexcept
    {
        return offset != 0 || signature != 0;
    }

    /**
     * @brief Resolves the relative offset into an absolute memory pointer using
     * the provided MemoryView base address.
     */
    template <typename T = void>
    [[nodiscard]]
    T *resolve(storage::MemoryView &view) const noexcept
    {
        if(!is_valid()) return nullptr;
        // todo validate allocator signature
        return view.cast_view<T>(offset);
    }

    constexpr bool operator==(const MemoryHandle &) const noexcept = default;
};

static_assert(
    sizeof(MemoryHandle) == sizeof(std::uint64_t),
    "MemoryHandle must be exactly 64 bits to fit tightly in component data.");
static_assert(
    std::is_trivially_copyable_v<MemoryHandle>,
    "MemoryHandle must be trivially copyable for SIMD/GPU DMA.");
} // namespace usagi::runtime
