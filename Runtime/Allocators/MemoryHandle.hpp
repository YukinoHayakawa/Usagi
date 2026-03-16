#pragma once

#include <span>

#include <Usagi/Runtime/Errors/Errors.hpp>
#include <Usagi/Runtime/Storage/Views/MemoryView.hpp>

namespace usagi::runtime
{
/**
 * Shio:
 * Represents the universal handle returned by all Layer 3 allocators.
 * In a heterogeneous DOD architecture, we cannot store raw pointers.
 * This handle encapsulates a 4-bit signature (identifying the allocator type)
 * and a 60-bit offset relative to the base of the MemoryView, plus a 64-bit
 * size field for bounds checking and span generation.
 */
struct MemoryHandle
{
    // allocator type bits
    std::uint64_t signature : 4  = 0;
    std::uint64_t offset    : 60 = 0;
    // I kinda want to pack more info here to note the object type, etc.
    std::uint64_t size           = 0;

    constexpr MemoryHandle() noexcept = default;

    constexpr MemoryHandle(
        const std::uint64_t sig, const std::uint64_t off,
        const std::uint64_t sz) noexcept
        : signature(sig), offset(off), size(sz)
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

        if constexpr(!std::is_void_v<T>)
        {
            USAGI_CHECK_FATAL(
                sizeof(T) <= size,
                "Resolved type exceeds the allocated memory bounds.");
        }

        return view.cast_view<T>(offset);
    }

    /**
     * @brief Resolves the allocation into a statically-typed contiguous memory
     * span.
     */
    template <typename T>
    [[nodiscard]]
    std::span<T> resolve_span(storage::MemoryView &view) const noexcept
    {
        if(!is_valid()) return std::span<T>();

        // Shio: We permit arbitrary resolutions over the memory region (e.g.,
        // viewing as bytes, or mapping to a smaller struct), so we simply
        // truncate the span extent to fit safely within the allocated size
        // without hard-trapping for exact divisibility.
        return std::span<T>(view.cast_view<T>(offset), size / sizeof(T));
    }

    constexpr bool operator==(const MemoryHandle &) const noexcept = default;
};

static_assert(
    sizeof(MemoryHandle) == 16,
    "MemoryHandle must be exactly 128 bits to fit tightly in component data.");
static_assert(
    std::is_trivially_copyable_v<MemoryHandle>,
    "MemoryHandle must be trivially copyable for SIMD/GPU DMA.");
} // namespace usagi::runtime
