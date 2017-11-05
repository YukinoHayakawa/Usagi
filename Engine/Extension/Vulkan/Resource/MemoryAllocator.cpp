#include <Usagi/Engine/Runtime/Memory/MemoryAllocator.hpp>
#include <Usagi/Engine/Runtime/Exception.hpp>

#include "MemoryAllocator.hpp"

namespace yuki::vulkan
{

MemoryAllocator::MemoryAllocator(size_t size, vk::Device device,
    vk::PhysicalDevice physical_device, std::unique_ptr<yuki::MemoryAllocator> allocator)
    : mDevice(device)
    , mAllocator { std::move(allocator) }
{
    vk::BufferCreateInfo buffer_create_info;
    buffer_create_info.setSize(size);
    // it's recommended to sub-allocate a large buffer for multiple types of buffers
    // https://developer.nvidia.com/vulkan-memory-management
    buffer_create_info.setUsage(
        vk::BufferUsageFlagBits::eTransferDst |
        vk::BufferUsageFlagBits::eVertexBuffer |
        vk::BufferUsageFlagBits::eIndexBuffer |
        vk::BufferUsageFlagBits::eUniformBuffer
    );
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

            // persistent mapping
            mMappedAddress = mDevice.mapMemory(mMemory.get(), 0, mSize, { });
            return;
        }
    }

    throw GraphicsDeviceMemoryAllocationFailureException() << SizeInfo(size);
}

MemoryAllocator::~MemoryAllocator()
{
    mDevice.unmapMemory(mMemory.get());
}

graphics::MemoryAllocator::Allocation MemoryAllocator::allocate(size_t size)
{
    return { 0, mAllocator->allocate(size, mAlignment), size };
}

void MemoryAllocator::release(const graphics::MemoryAllocator::Allocation &alloc_info)
{
    mAllocator->release(alloc_info.offset);
}

graphics::MemoryAllocator::Allocation MemoryAllocator::reallocate(const Allocation &alloc_info)
{
    return alloc_info;
}

void MemoryAllocator::reset()
{
    mAllocator->reset();
}

vk::Buffer MemoryAllocator::_getBuffer() const
{
    return mBuffer.get();
}

void * MemoryAllocator::getMappedAddress(const graphics::MemoryAllocator::Allocation &allocation) const
{
    return static_cast<char*>(mMappedAddress) + allocation.offset;
}

void MemoryAllocator::flushRange(const graphics::MemoryAllocator::Allocation &allocation, size_t offset, size_t size)
{
    vk::MappedMemoryRange mapped_memory_range;
    mapped_memory_range.setMemory(mMemory.get());
    mapped_memory_range.setOffset(allocation.offset + offset);
    mapped_memory_range.setSize(size);
    mDevice.flushMappedMemoryRanges({ mapped_memory_range });
}

graphics::MemoryAllocator * MemoryAllocator::getActualAllocator(const graphics::MemoryAllocator::Allocation &allocation)
{
    return this;
}

}
