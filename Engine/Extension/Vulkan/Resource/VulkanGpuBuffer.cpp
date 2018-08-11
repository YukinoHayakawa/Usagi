#include "VulkanGpuBuffer.hpp"

#include <Usagi/Engine/Extension/Vulkan/VulkanGpuDevice.hpp>

#include "VulkanBufferAllocation.hpp"
#include "VulkanMemoryPool.hpp"

usagi::VulkanGpuBuffer::VulkanGpuBuffer(VulkanMemoryPool *pool)
    : mPool(pool)
{
}

void usagi::VulkanGpuBuffer::allocate(std::size_t size)
{
    mAllocation = mPool->allocate(size);
}

void * usagi::VulkanGpuBuffer::getMappedMemory()
{
    return mAllocation->mappedAddress();
}

void usagi::VulkanGpuBuffer::flush()
{
    vk::MappedMemoryRange range;
    range.setMemory(mPool->memory());
    range.setOffset(mAllocation->offset());
    range.setSize(mAllocation->size());

    mPool->device()->device().flushMappedMemoryRanges({ range });
}
