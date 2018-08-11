#include "VulkanMemoryPool.hpp"

#include <Usagi/Engine/Core/Logging.hpp>
#include <Usagi/Engine/Extension/Vulkan/VulkanGpuDevice.hpp>

#include "VulkanBufferAllocation.hpp"

void usagi::VulkanMemoryPool::createBuffer(
    std::size_t size,
    const vk::BufferUsageFlags &usages)
{
    vk::BufferCreateInfo buffer_create_info;

    buffer_create_info.setSize(size);
    buffer_create_info.setUsage(usages);
    buffer_create_info.setSharingMode(vk::SharingMode::eExclusive);

    mBuffer = mDevice->device().createBufferUnique(buffer_create_info);
}

void usagi::VulkanMemoryPool::allocateDeviceMemory(
    std::size_t size,
    const vk::MemoryPropertyFlags &mem_properties,
    const vk::BufferUsageFlags &usages)
{
    createBuffer(size, usages);
    const auto buffer_memory_requirements =
        mDevice->device().getBufferMemoryRequirements(mBuffer.get());
    const auto memory_properties =
        mDevice->physicalDevice().getMemoryProperties();

    // find first heap
    for(uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
    {
        // the i-th bit is set only when that memory type is supported.
        // https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkMemoryRequirements.html
        if((buffer_memory_requirements.memoryTypeBits & 1 << i) &&
            (memory_properties.memoryTypes[i].propertyFlags & mem_properties))
        {
            vk::MemoryAllocateInfo info;
            info.setAllocationSize(buffer_memory_requirements.size);
            info.setMemoryTypeIndex(i);

            try
            {
                mMemory = mDevice->device().allocateMemoryUnique(info);
                mDevice->device().bindBufferMemory(
                    mBuffer.get(), mMemory.get(), 0);
                mAllocator = std::make_unique<BitmapMemoryAllocator>(
                    nullptr,
                    buffer_memory_requirements.size,
                    8 * 1024 /* 8 KiB */, // todo config
                    buffer_memory_requirements.alignment
                );
                return;
            }
            catch(const vk::OutOfHostMemoryError &e)
            {
                LOG(warn, "Host memory is exhausted: {}", e.what());
            }
            catch(const vk::OutOfDeviceMemoryError &e)
            {
                LOG(warn, "GPU memory heap {} is exhausted: {}",
                    memory_properties.memoryTypes[i].heapIndex, e.what());
            }
        }
    }
    throw std::bad_alloc();
}

usagi::VulkanMemoryPool::VulkanMemoryPool(
    VulkanGpuDevice *device,
    std::size_t size,
    const vk::MemoryPropertyFlags &mem_properties,
    const vk::BufferUsageFlags &usages)
    : mDevice(device)
{
    allocateDeviceMemory(size, mem_properties, usages);

    mMappedMemory = static_cast<char*>(mDevice->device().mapMemory(
        mMemory.get(), 0, VK_WHOLE_SIZE));
}

usagi::VulkanMemoryPool::~VulkanMemoryPool()
{
    if(mMappedMemory != nullptr)
        mDevice->device().unmapMemory(mMemory.get());
    mMappedMemory = nullptr;
}

std::shared_ptr<usagi::VulkanBufferAllocation> usagi::VulkanMemoryPool::
    allocate(std::size_t size)
{
    auto offset = reinterpret_cast<std::size_t>(mAllocator->allocate(size));

    try
    {
        auto alloc = std::make_shared<VulkanBufferAllocation>(
            this,
            offset, size,
            mMappedMemory + offset
        );
        return std::move(alloc);
    }
    catch(...)
    {
        mAllocator->deallocate(reinterpret_cast<void*>(offset));
        throw;
    }
}

void usagi::VulkanMemoryPool::deallocate(std::size_t offset)
{
    mAllocator->deallocate(reinterpret_cast<void*>(offset));
}
