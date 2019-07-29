#pragma once

#include <vulkan/vulkan.hpp>

#include "VulkanBatchResource.hpp"

namespace usagi
{
class VulkanBufferMemoryPoolBase;

class VulkanBufferAllocation : public VulkanBatchResource
{
    VulkanBufferMemoryPoolBase * mPool = nullptr;
    const std::size_t mOffset, mSize;
    // nullptr if the buffer is not mapped
    void * const mMappedAddress = nullptr;

public:
    VulkanBufferAllocation(
        VulkanBufferMemoryPoolBase *pool,
        std::size_t offset,
        std::size_t size,
        void *mapped_address);
    ~VulkanBufferAllocation();

    VulkanBufferMemoryPoolBase * pool() const { return mPool; }
    std::size_t offset() const { return mOffset; }
    std::size_t size() const { return mSize; }
    void * mappedAddress() const { return mMappedAddress; }
};
}
