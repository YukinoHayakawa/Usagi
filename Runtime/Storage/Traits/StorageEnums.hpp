#pragma once

#include <Usagi/Library/Constants/BitWidth.hpp>
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
    // Standard byte-aligned data (strings, chars)
    _1Byte   = std::to_underlying(BitWidth::_1Byte),
    // 16-bit values (short, half-float)
    _2Byte   = std::to_underlying(BitWidth::_2Byte),
    // 32-bit values (int, float)
    _4Byte   = std::to_underlying(BitWidth::_4Byte),
    // 64-bit values (double, long, raw pointers)
    _8Byte   = std::to_underlying(BitWidth::_8Byte),
    // SSE/SIMD vectors (float4, mat4)
    _16Byte  = std::to_underlying(BitWidth::_16Byte),
    // AVX-256 vectors
    _32Byte  = std::to_underlying(BitWidth::_32Byte),
    // CPU Cache Line size (Prevents False Sharing)
    _64Byte  = std::to_underlying(BitWidth::_64Byte),
    // AVX-512 / Advanced DMA alignment
    _128Byte = std::to_underlying(BitWidth::_128Byte),
    // GPU Texture/Buffer pitch requirements
    _256Byte = std::to_underlying(BitWidth::_256Byte),
    // Sector-level alignment
    _512Byte = std::to_underlying(BitWidth::_512Byte),
    // Common for large block-based structures
    _1KiB    = std::to_underlying(BitWidth::_1KiB),
    // Standard OS Virtual Page alignment
    _4KiB    = std::to_underlying(BitWidth::_4KiB),
    // Win32 VirtualAlloc reservation granularity
    _64KiB   = std::to_underlying(BitWidth::_64KiB),
    // LTO-7+ optimized block alignment
    _256KiB  = std::to_underlying(BitWidth::_256KiB),
    // Large archival block
    _1MiB    = std::to_underlying(BitWidth::_1MiB),
    // Enterprise tape / high-throughput streaming
    _8MiB    = std::to_underlying(BitWidth::_8MiB),

    // LTO specialized transfer alignment
    _480KiB = std::to_underlying(BitWidth::_480KiB),
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
    // Standard x86/ARM page size
    _4KiB  = std::to_underlying(BitWidth::_4KiB),
    // Common on Apple Silicon / Advanced OS configurations
    _16KiB = std::to_underlying(BitWidth::_16KiB),
    // Efficient for large file mapping (NTFS cluster default)
    _64KiB = std::to_underlying(BitWidth::_64KiB),
    // Medium "Huge Page"
    _1MiB  = std::to_underlying(BitWidth::_1MiB),
    // Standard x86 Large Page (High TLB efficiency)
    _2MiB  = std::to_underlying(BitWidth::_2MiB),
    // Large page variant
    _4MiB  = std::to_underlying(BitWidth::_4MiB),
    // Large LTO streaming page
    _8MiB  = std::to_underlying(BitWidth::_8MiB),
    // Very large pages for specialized hardware
    _16MiB = std::to_underlying(BitWidth::_16MiB),
    // x86 Gigapage (Zero TLB overhead for massive heaps)
    _1GiB  = std::to_underlying(BitWidth::_1GiB),

    // LTO specialized transfer page size
    _480KiB = std::to_underlying(BitWidth::_480KiB),
};
} // namespace usagi::runtime::storage

namespace usagi
{
template <>
struct IsBitWidthEnum<runtime::storage::StorageAlignment> : std::true_type
{
};

template <>
struct IsBitWidthEnum<runtime::storage::StoragePageSize> : std::true_type
{
};
} // namespace usagi

namespace usagi::runtime::storage
{
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

namespace details::static_tests
{
/* Shio: Verifying specialized alignment and operands */
static_assert(to_bytes(StorageAlignment::_480KiB) == 491'520);
static_assert(to_bytes(StoragePageSize::_4KiB) == 4'096);
} // namespace details::static_tests
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
