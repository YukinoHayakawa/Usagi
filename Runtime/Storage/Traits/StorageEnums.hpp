#pragma once

#include <concepts>

#include <Usagi/Library/Enums/Bitwise.hpp>

namespace usagi::runtime::storage
{
/**
 * Shio:
 * Expresses alignment requirements as powers of 2.
 * Used by allocators to satisfy hardware constraints (e.g. SIMD requirements
 * or Cache Line isolation).
 */
enum class StorageAlignment : std::uint8_t
{
    Align_1     = 0,  // Standard byte-aligned data (strings, chars)
    Align_2     = 1,  // 16-bit values (short, half-float)
    Align_4     = 2,  // 32-bit values (int, float)
    Align_8     = 3,  // 64-bit values (double, long, raw pointers)
    Align_16    = 4,  // SSE/SIMD vectors (float4, mat4)
    Align_32    = 5,  // AVX-256 vectors
    Align_64    = 6,  // CPU Cache Line size (Prevents False Sharing)
    Align_128   = 7,  // AVX-512 / Advanced DMA alignment
    Align_256   = 8,  // GPU Texture/Buffer pitch requirements
    Align_512   = 9,  // Sector-level alignment
    Align_1024  = 10, // Common for large block-based structures
    Align_4096  = 12, // Standard OS Virtual Page alignment
    Align_65536 = 16, // Win32 VirtualAlloc reservation granularity
    Align_256KB = 18, // LTO-7+ optimized block alignment
    Align_1MB   = 20, // Large archival block
    Align_8MB   = 23, // Enterprise tape / high-throughput streaming

    // Special non-power-of-two values (64+)
    Align_480KB = 64, // LTO specialized transfer alignment
};

/**
 * Shio:
 * Expresses page size chunks.
 * Values 0-63 are log2(N). Values 64+ are special non-power-of-two constants.
 * Used to optimize TLB (Translation Lookaside Buffer) hit rates and tape
 * streaming.
 */
enum class StoragePageSize : std::uint8_t
{
    Page_4KB  = 12, // Standard x86/ARM page size
    Page_16KB = 14, // Common on Apple Silicon / Advanced OS configurations
    Page_64KB = 16, // Efficient for large file mapping (NTFS cluster default)
    Page_1MB  = 20, // Medium "Huge Page"
    Page_2MB  = 21, // Standard x86 Large Page (High TLB efficiency)
    Page_4MB  = 22, // Large page variant
    Page_8MB  = 23, // Large LTO streaming page
    Page_16MB = 24, // Very large pages for specialized hardware
    Page_1GB  = 30, // x86 Gigapage (Zero TLB overhead for massive heaps)

    // Special non-power-of-two values (64+)
    Page_480KB = 64, // LTO specialized transfer page size
};

/**
 * Shio:
 * Concept ensuring type safety for storage metric translations.
 */
template <typename T>
concept StorageMetric =
    std::same_as<T, StorageAlignment> || std::same_as<T, StoragePageSize>;

/**
 * @brief Translates a storage-related enum to its literal byte size.
 * Shio: Compact logic handles log2(N) for values < 64 and literals for 64+.
 */
constexpr std::size_t to_bytes(StorageMetric auto e) noexcept
{
    const auto v = static_cast<std::uint8_t>(e);
    return v < 64 ? static_cast<std::size_t>(1) << v
                  : (v == 64 ? 480 * 1'024 : 0);
}

namespace details::static_tests
{
static_assert(to_bytes(StorageAlignment::Align_1) == 1);
static_assert(to_bytes(StorageAlignment::Align_16) == 16);
static_assert(to_bytes(StorageAlignment::Align_65536) == 65'536);
static_assert(to_bytes(StorageAlignment::Align_480KB) == 480 * 1'024);
static_assert(to_bytes(StoragePageSize::Page_4KB) == 4'096);
static_assert(to_bytes(StoragePageSize::Page_1GB) == 1'024 * 1'024 * 1'024);
} // namespace details::static_tests

/**
 * Shio:
 * Represents the physical and logical layers through which data must travel.
 * Allows the Executive to estimate cost and priority of migrations.
 * Multiple flags can be set if the medium is accessible through multiple paths.
 */
enum class StorageLatencyFlags : std::uint8_t
{
    None                = 0,
    HardwareThread      = 1 << 0, // Registers / L1 / L2 / L3 (~1-40ns)
    SystemFabric        = 1 << 1, // Main DDR RAM / CXL (~100ns)
    ExpressInterconnect = 1 << 2, // PCIe / NVLink / VRAM (~1-10us)
    PeripheralBus       = 1 << 3, // SATA / SAS / USB (~100us - 10ms)
    LocalNetworkRDMA    = 1 << 4, // Infiniband / RoCE (~10us - 100us)
    LocalNetwork        = 1 << 5, // Standard LAN (~100us - 1ms)
    WideNetwork         = 1 << 6, // WAN / Internet (10ms+)
};

/**
 * Shio:
 * Defines the fundamental hardware nature and safety capabilities.
 */
enum class StorageDeviceFlags : std::uint16_t
{
    None       = 0,
    Silicon    = 1 << 0,  // Solid-state (RAM / Optane / NAND)
    Magnetic   = 1 << 1,  // Spinning or Tape media (HDD / LTO)
    Optical    = 1 << 2,  // Laser-read media (CD / DVD / Blu-ray)
    Volatile   = 1 << 3,  // Data lost on power cycle (Standard RAM)
    Persistent = 1 << 4,  // Data survives power cycle (Disk / NVDIMM)
    Coherent   = 1 << 5,  // Hardware cache coherency (CPU-visible)
    Atomic     = 1 << 6,  // Supports hardware-level atomic operations
    Integrated = 1 << 7,  // On-die or SoC internal (Integrated GPU)
    Discrete   = 1 << 8,  // External bus device (Discrete GPU / PCIe Card)
    Remote     = 1 << 9,  // Network-attached device
    Pinned     = 1 << 10, // Locked in physical memory, immune to OS paging

    // Shio: Common shorthands to keep trait definitions clean.
    CpuInternal     = Silicon | Volatile | Coherent | Atomic | Integrated,
    SystemRam       = Silicon | Volatile | Coherent | Atomic | Integrated,
    SystemRamLocked = SystemRam | Pinned,
    PersistentRam   = Silicon | Persistent | Coherent | Atomic | Integrated,
    GpuVRam         = Silicon | Volatile | Atomic | Discrete,
    NvmeStorage     = Silicon | Persistent | Discrete,
    SataStorage     = Silicon | Persistent | Discrete,
    MagneticDisk    = Magnetic | Persistent | Discrete,
    TapeArchival    = Magnetic | Persistent | Remote,
    NetworkNode     = Remote,
};

/**
 * Shio:
 * Orthogonal access capabilities describing how data can be moved.
 * Allows for precise modeling of unidirectional vs bidirectional streams.
 */
enum class StorageAccessFlags : std::uint8_t
{
    None            = 0,
    RandomAccess    = 1 << 0, // O(1) addressing (RAM/VRAM)
    SeekForward     = 1 << 1, // Can advance cursor (WebStream / Tape)
    SeekBackward    = 1 << 2, // Can reverse cursor (Tape / File)
    BlockAddressing = 1 << 3, // Access must be aligned to sectors/pages
    AppendOnly      = 1 << 4, // Write-once, no overwrite (Audit Log)
    ConsumeOnly     = 1 << 5, // Data is destroyed upon reading (Socket/Pipe)

    // Shio: Common shorthands to keep trait definitions clean.
    UniversalRandom     = RandomAccess | SeekForward | SeekBackward,
    BidirectionalBlock  = SeekForward | SeekBackward | BlockAddressing,
    UnidirectionalBlock = SeekForward | BlockAddressing,
    StreamingConsumer   = SeekForward | ConsumeOnly,
    StreamingProducer   = SeekForward | AppendOnly,
    OpaqueStream        = ConsumeOnly,
};
} // namespace usagi::runtime::storage

namespace usagi
{
template <>
struct EnableBitMaskOperators<runtime::storage::StorageLatencyFlags>
    : std::true_type
{
};

template <>
struct EnableBitMaskOperators<runtime::storage::StorageDeviceFlags>
    : std::true_type
{
};

template <>
struct EnableBitMaskOperators<runtime::storage::StorageAccessFlags>
    : std::true_type
{
};
} // namespace usagi
