#pragma once

#include <vulkan/vulkan.hpp>

#include "VulkanBatchResource.hpp"

namespace usagi
{
class VulkanMemoryPool;

class VulkanBufferAllocation : public VulkanBatchResource
{
    VulkanMemoryPool * mPool = nullptr;
    const std::size_t mOffset, mSize;

    // Offset to base buffer is applied.
    void * const mMappedAddress = nullptr;

public:
    VulkanBufferAllocation(
        VulkanMemoryPool *pool,
        std::size_t offset,
        std::size_t size,
        void *apped_address);
    ~VulkanBufferAllocation();

    VulkanMemoryPool * pool() const { return mPool; }
    std::size_t offset() const { return mOffset; }
    std::size_t size() const { return mSize; }
    void * mappedAddress() const { return mMappedAddress; }
};
}
