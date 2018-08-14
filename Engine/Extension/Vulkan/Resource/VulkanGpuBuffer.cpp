#include "VulkanGpuBuffer.hpp"

#include <Usagi/Engine/Extension/Vulkan/VulkanGpuDevice.hpp>

#include "VulkanBufferAllocation.hpp"
#include "VulkanMemoryPool.hpp"

usagi::VulkanGpuBuffer::VulkanGpuBuffer(
    VulkanBufferMemoryPoolBase *pool,
    const GpuBufferUsage usage)
    : mPool(pool)
    , mUsage(usage)
{
}

void usagi::VulkanGpuBuffer::allocate(std::size_t size)
{
    mAllocation = mPool->allocate(size);
}

void * usagi::VulkanGpuBuffer::mappedMemory()
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

void usagi::VulkanGpuBuffer::fillShaderResourceInfo(
    vk::WriteDescriptorSet &write,
    VulkanResourceInfo &info)
{
    auto &buffer_info = std::get<vk::DescriptorBufferInfo>(info);
    buffer_info.setBuffer(mAllocation->pool()->buffer());
    buffer_info.setOffset(mAllocation->offset());
    buffer_info.setRange(mAllocation->size());
    write.setPBufferInfo(&buffer_info);
}
