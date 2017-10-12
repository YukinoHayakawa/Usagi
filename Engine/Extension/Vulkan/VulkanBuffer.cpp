#include <Usagi/Engine/Runtime/Exception.hpp>

#include "VulkanBuffer.hpp"

yuki::VulkanBuffer::VulkanBuffer(vk::BufferUsageFlags usage, size_t size, vk::Device device, vk::PhysicalDevice physical_device)
    : mDevice(device)
{
    vk::BufferCreateInfo buffer_create_info;
    buffer_create_info.setSize(size);
    buffer_create_info.setUsage(usage);
    buffer_create_info.setSharingMode(vk::SharingMode::eExclusive);
    mBuffer = device.createBufferUnique(buffer_create_info);

    const auto buffer_memory_requirements = device.getBufferMemoryRequirements(mBuffer.get());
    const auto memory_properties = physical_device.getMemoryProperties();

    for(uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
    {
        if((buffer_memory_requirements.memoryTypeBits & (1 << i)) &&
            (memory_properties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible))
        {
            vk::MemoryAllocateInfo memory_allocate_info;
            memory_allocate_info.setAllocationSize(buffer_memory_requirements.size);
            memory_allocate_info.setMemoryTypeIndex(i);

            mMemory = device.allocateMemoryUnique(memory_allocate_info);
            mDevice.bindBufferMemory(mBuffer.get(), mMemory.get(), 0);

            mAlignment = buffer_memory_requirements.alignment;
            mSize = size;

            return;
        }
    }

    throw GraphicsDeviceMemoryAllocationFailureException() << SizeInfo(size);
}

void * yuki::VulkanBuffer::map(size_t offset, size_t size)
{
    if(offset + size > mSize) throw MemoryAccessOutOfBoundException();
    const auto mapped_address = mDevice.mapMemory(mMemory.get(), offset, size, { });
    mIsMapped = true;
    mMappingOffset = offset;
    return mapped_address;
}

void yuki::VulkanBuffer::unmap()
{
    if(!mIsMapped) throw UnmappingUnmappedMemoryRangeException();

    vk::MappedMemoryRange mapped_memory_range;
    mapped_memory_range.setMemory(mMemory.get());
    mapped_memory_range.setOffset(mMappingOffset);
    mapped_memory_range.setSize(VK_WHOLE_SIZE);
    mDevice.flushMappedMemoryRanges({ mapped_memory_range });
    mDevice.unmapMemory(mMemory.get());

    mIsMapped = false;
    mMappingOffset = 0;
}

vk::Buffer yuki::VulkanBuffer::_getBuffer() const
{
    return mBuffer.get();
}
