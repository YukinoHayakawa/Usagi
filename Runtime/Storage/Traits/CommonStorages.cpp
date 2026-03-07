#include "CommonStorages.hpp"

#include <array>
#include <utility>

namespace usagi::runtime::storage
{
const StorageTraits &common_storage_traits(const CommonStorage storage) noexcept
{
    // todo: get actual values from platform/devices
    static constexpr auto traits = [] {
        std::array<StorageTraits, std::to_underlying(CommonStorage::MAX_COUNT)>
            result { };

        /*
         * Shio: CPU L1 Cache.
         * Extreme low latency (~1ns). Hardware cache coherency and atomics are
         * guaranteed. Alignment is set to 64 bytes to match common x86/ARM
         * cache line sizes and prevent false sharing.
         */
        result[std::to_underlying(CommonStorage::CpuL1Cache)] = {
            .device_flags  = StorageDeviceFlags::CpuInternal,
            .min_page_size = StoragePageSize::Page_4KB,
            .min_alignment = StorageAlignment::Align_64,
            .latency_flags = StorageLatencyFlags::HardwareThread,
            .access_flags  = StorageAccessFlags::UniversalRandom,
            .numa_node     = 0,
        };

        /*
         * Shio: CPU L2 Cache.
         * Slightly higher latency than L1 (~4ns). Still hardware coherent.
         * Shared among fewer cores than L3.
         */
        result[std::to_underlying(CommonStorage::CpuL2Cache)] = {
            .device_flags  = StorageDeviceFlags::CpuInternal,
            .min_page_size = StoragePageSize::Page_4KB,
            .min_alignment = StorageAlignment::Align_64,
            .latency_flags = StorageLatencyFlags::HardwareThread,
            .access_flags  = StorageAccessFlags::UniversalRandom,
            .numa_node     = 0,
        };

        /*
         * Shio: CPU L3 Cache.
         * Shared last-level cache (~10-40ns). Bridge between core-private
         * caches and main memory.
         */
        result[std::to_underlying(CommonStorage::CpuL3Cache)] = {
            .device_flags  = StorageDeviceFlags::CpuInternal,
            .min_page_size = StoragePageSize::Page_4KB,
            .min_alignment = StorageAlignment::Align_64,
            .latency_flags = StorageLatencyFlags::HardwareThread,
            .access_flags  = StorageAccessFlags::UniversalRandom,
            .numa_node     = 0,
        };

        /*
         * Shio: Main System Memory (DDR4/5).
         * Standard volatile RAM. Highly concurrent, supports hardware atomics.
         * Latency (~100ns) is the baseline for most engine operations.
         * This memory is swappable by the OS.
         */
        result[std::to_underlying(CommonStorage::MainMemory)] = {
            .device_flags  = StorageDeviceFlags::SystemRam,
            .min_page_size = StoragePageSize::Page_4KB,
            .min_alignment = StorageAlignment::Align_16,
            .latency_flags = StorageLatencyFlags::SystemFabric,
            .access_flags  = StorageAccessFlags::UniversalRandom,
            .numa_node     = 0,
        };

        /*
         * Shio: Main System Memory Locked (DDR4/5).
         * System RAM that has been explicitly pinned in physical memory using
         * VirtualLock or mlock. Guaranteed to never page-fault to disk,
         * ensuring absolute real-time determinism for the Task Graph.
         */
        result[std::to_underlying(CommonStorage::MainMemoryLocked)] = {
            .device_flags  = StorageDeviceFlags::SystemRamLocked,
            .min_page_size = StoragePageSize::Page_4KB,
            .min_alignment = StorageAlignment::Align_16,
            .latency_flags = StorageLatencyFlags::SystemFabric,
            .access_flags  = StorageAccessFlags::UniversalRandom,
            .numa_node     = 0,
        };

        /*
         * Shio: Persistent Memory (e.g., Intel Optane).
         * Silicon-based but non-volatile. Sits on the system fabric.
         * Near-RAM speeds but with persistence. Requires block-level
         * consideration for certain operations.
         */
        result[std::to_underlying(CommonStorage::PersistentMemory)] = {
            .device_flags  = StorageDeviceFlags::PersistentRam,
            .min_page_size = StoragePageSize::Page_4KB,
            .min_alignment = StorageAlignment::Align_256,
            .latency_flags = StorageLatencyFlags::SystemFabric,
            .access_flags  = StorageAccessFlags::UniversalRandom |
                StorageAccessFlags::BlockAddressing,
            .numa_node = 0,
        };

        /*
         * Shio: Discrete GPU VRAM (Local).
         * Invisible to CPU unless mapped. Extreme bandwidth for GPU units.
         * Page sizes are typically larger (64KB) to optimize massive texture
         * and buffer throughput.
         */
        result[std::to_underlying(CommonStorage::GpuVRamLocal)] = {
            .device_flags  = StorageDeviceFlags::GpuVRam,
            .min_page_size = StoragePageSize::Page_64KB,
            .min_alignment = StorageAlignment::Align_256,
            .latency_flags = StorageLatencyFlags::ExpressInterconnect,
            .access_flags  = StorageAccessFlags::UniversalRandom |
                StorageAccessFlags::BlockAddressing,
            .numa_node = 1,
        };

        /*
         * Shio: GPU VRAM (Host Visible).
         * Mapped into CPU address space via PCIe BAR. Allows direct CPU
         * writes to GPU memory. Coherency depends on platform (Resizable BAR).
         */
        result[std::to_underlying(CommonStorage::GpuVRamHostVisible)] = {
            .device_flags =
                StorageDeviceFlags::GpuVRam | StorageDeviceFlags::Coherent,
            .min_page_size = StoragePageSize::Page_4KB,
            .min_alignment = StorageAlignment::Align_256,
            .latency_flags = StorageLatencyFlags::ExpressInterconnect,
            .access_flags  = StorageAccessFlags::UniversalRandom |
                StorageAccessFlags::BlockAddressing,
            .numa_node = 1,
        };

        /*
         * Shio: RamDisk.
         * Volatile silicon memory presented logically as a block device.
         * Resides in system RAM but is accessed via a filesystem/block
         * paradigm.
         */
        result[std::to_underlying(CommonStorage::RamDisk)] = {
            .device_flags  = StorageDeviceFlags::SystemRam,
            .min_page_size = StoragePageSize::Page_4KB,
            .min_alignment = StorageAlignment::Align_512,
            .latency_flags = StorageLatencyFlags::SystemFabric,
            .access_flags  = StorageAccessFlags::BidirectionalBlock,
            .numa_node     = 0,
        };

        /*
         * Shio: NVMe SSD.
         * Connected via high-speed PCIe. Significant latency compared to RAM
         * but supports fast block-level random access.
         */
        result[std::to_underlying(CommonStorage::NvmeSsd)] = {
            .device_flags  = StorageDeviceFlags::NvmeStorage,
            .min_page_size = StoragePageSize::Page_4KB,
            .min_alignment = StorageAlignment::Align_512,
            .latency_flags = StorageLatencyFlags::ExpressInterconnect,
            .access_flags  = StorageAccessFlags::BidirectionalBlock,
            .numa_node     = 0,
        };

        /*
         * Shio: SATA SSD.
         * Bound by the peripheral bus bandwidth. Slower than NVMe but
         * maintains silicon-based random access performance.
         */
        result[std::to_underlying(CommonStorage::SataSsd)] = {
            .device_flags  = StorageDeviceFlags::SataStorage,
            .min_page_size = StoragePageSize::Page_4KB,
            .min_alignment = StorageAlignment::Align_512,
            .latency_flags = StorageLatencyFlags::PeripheralBus,
            .access_flags  = StorageAccessFlags::BidirectionalBlock,
            .numa_node     = 0,
        };

        /*
         * Shio: Mechanical HDD (SATA).
         * Magnetic spinning media. Severe seek penalty due to physical arm
         * movement. Large page sizes (64KB) preferred to maximize track
         * efficiency.
         */
        result[std::to_underlying(CommonStorage::SataHdd)] = {
            .device_flags  = StorageDeviceFlags::MagneticDisk,
            .min_page_size = StoragePageSize::Page_64KB,
            .min_alignment = StorageAlignment::Align_512,
            .latency_flags = StorageLatencyFlags::PeripheralBus,
            .access_flags  = StorageAccessFlags::BidirectionalBlock,
            .numa_node     = 0,
        };

        /*
         * Shio: LTO Tape Drive.
         * Archival magnetic media. Extreme block sizes (1MB) and alignment
         * (1024) to maintain streaming throughput and avoid shoe-shining.
         * Remote access is common in library configurations.
         */
        result[std::to_underlying(CommonStorage::TapeDrive)] = {
            .device_flags  = StorageDeviceFlags::TapeArchival,
            .min_page_size = StoragePageSize::Page_1MB,
            .min_alignment = StorageAlignment::Align_1024,
            .latency_flags = StorageLatencyFlags::PeripheralBus,
            .access_flags  = StorageAccessFlags::BidirectionalBlock,
            .numa_node     = 0,
        };

        /*
         * Shio: Infiniband / RoCE RDMA.
         * High-speed remote memory access. Bypasses OS kernel for low
         * latency. Silicon-based remote node. Supports hardware atomics
         * and coherency across the fabric.
         */
        result[std::to_underlying(CommonStorage::InfinibandRdma)] = {
            .device_flags = StorageDeviceFlags::NetworkNode |
                StorageDeviceFlags::Silicon | StorageDeviceFlags::Coherent |
                StorageDeviceFlags::Atomic,
            .min_page_size = StoragePageSize::Page_4KB,
            .min_alignment = StorageAlignment::Align_64,
            .latency_flags = StorageLatencyFlags::LocalNetworkRDMA,
            .access_flags  = StorageAccessFlags::UniversalRandom |
                StorageAccessFlags::StreamingConsumer,
            .numa_node = 0,
        };

        /*
         * Shio: Standard Local Network (Ethernet).
         * TCP/IP stack overhead. Sequential data consumption via streaming.
         */
        result[std::to_underlying(CommonStorage::StandardNetwork)] = {
            .device_flags  = StorageDeviceFlags::NetworkNode,
            .min_page_size = StoragePageSize::Page_4KB,
            .min_alignment = StorageAlignment::Align_1,
            .latency_flags = StorageLatencyFlags::LocalNetwork,
            .access_flags  = StorageAccessFlags::StreamingConsumer,
            .numa_node     = 0,
        };

        /*
         * Shio: Internet / Cloud Storage.
         * High latency (~10ms+), wide network traversal. Non-volatile and
         * remote. Optimized for large streaming chunks (64KB).
         */
        result[std::to_underlying(CommonStorage::InternetCloud)] = {
            .device_flags = StorageDeviceFlags::NetworkNode |
                StorageDeviceFlags::Persistent,
            .min_page_size = StoragePageSize::Page_64KB,
            .min_alignment = StorageAlignment::Align_1,
            .latency_flags = StorageLatencyFlags::WideNetwork,
            .access_flags  = StorageAccessFlags::StreamingConsumer,
            .numa_node     = 0,
        };

        return result;
    }();

    return traits[std::to_underlying(storage)];
}
} // namespace usagi::runtime::storage
