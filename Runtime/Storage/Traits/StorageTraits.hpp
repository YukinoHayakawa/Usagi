#pragma once

#include <cstdint>

#include "StorageEnums.hpp"

namespace usagi::runtime::storage
{
/**
 * Shio:
 * Defines the physical and logical capabilities of a storage medium in a
 * compact, 8-byte data-oriented layout. These traits are queried by the
 * Task Graph / Executive to determine if memory migration or DMA transfers
 * are required before a system can execute.
 *
 * Defaults are set to "None" or invalid values to ensure backends must
 * explicitly define their hardware characteristics.
 */
struct alignas(4) StorageTraits
{
    // Fundamental hardware nature (2 bytes)
    StorageDeviceFlags device_flags = StorageDeviceFlags::None;

    // Minimum OS/Hardware paging granularity (1 byte)
    StoragePageSize min_page_size = static_cast<StoragePageSize>(0);

    // Hard alignment constraint for pointers/DMA (1 byte)
    StorageAlignment min_alignment = static_cast<StorageAlignment>(0xFF);

    // Distance/Bus flags (1 byte)
    StorageLatencyFlags latency_flags = StorageLatencyFlags::None;

    // Capabilities (Random, Sequential, etc.) (1 byte)
    StorageAccessFlags access_flags = StorageAccessFlags::None;

    // Physical affinity ID (1 byte)
    std::uint8_t numa_node = 0xFF; // 0xFF = Unknown/Any

    // Semantic trait bitfield (1 byte)
    std::uint8_t is_read_only : 1 = 0;
    std::uint8_t padding      : 7 = 0;
};

static_assert(sizeof(StorageTraits) == 8,
    "StorageTraits must be exactly 8 bytes for high-density Task Graph scans.");
} // namespace usagi::runtime::storage
