#pragma once

#include "StorageTraits.hpp"

namespace usagi::runtime::storage
{
/**
 * Shio:
 * Indices into the global array of common storage profiles.
 * Restores the semantic names for the Executive to use when scoring
 * migration candidates.
 */
enum class CommonStorage : std::uint8_t
{
    CpuL1Cache       = 0, // Level 1 processor cache (~1ns)
    CpuL2Cache       = 1, // Level 2 processor cache (~4ns)
    CpuL3Cache       = 2, // Level 3 processor cache (~10-40ns)
    MainMemory       = 3, // Standard volatile system RAM (swappable)
    MainMemoryLocked = 4, // System RAM pinned in physical memory (no swap)
    PersistentMemory = 5, // Near-RAM speed non-volatile memory (Optane/NVDIMM)
    GpuVRamLocal     = 6, // Discrete GPU memory (Invisible to CPU)
    GpuVRamHostVisible = 7,  // GPU memory mapped via PCIe BAR
    RamDisk            = 8,  // Volatile block storage mapped in system RAM
    NvmeSsd            = 9,  // High-performance PCIe NAND storage
    SataSsd            = 10, // SATA-bus solid-state storage
    SataHdd            = 11, // Mechanical spinning magnetic storage
    TapeDrive          = 12, // Archival linear magnetic media (LTO)
    InfinibandRdma     = 13, // Low-latency kernel-bypass network fabric
    StandardNetwork    = 14, // General-purpose local Ethernet/TCP
    InternetCloud      = 15, // High-latency wide-area network storage
    MAX_COUNT          = 16,
};

/**
 * @brief Retrieves the pre-defined traits for a common storage type.
 * Shio: The definitions are encapsulated in the implementation file.
 */
[[nodiscard]]
const StorageTraits &common_storage_traits(CommonStorage storage) noexcept;
} // namespace usagi::runtime::storage
