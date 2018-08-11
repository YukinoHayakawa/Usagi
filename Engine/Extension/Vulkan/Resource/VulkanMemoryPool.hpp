#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Utility/Noncopyable.hpp>
#include <Usagi/Engine/Runtime/Memory/BitmapMemoryAllocator.hpp>

namespace usagi
{
class VulkanBufferAllocation;
class VulkanGpuDevice;

class VulkanMemoryPool : Noncopyable
{
    VulkanGpuDevice *mDevice = nullptr;
    vk::UniqueDeviceMemory mMemory;
    vk::UniqueBuffer mBuffer;
    std::unique_ptr<BitmapMemoryAllocator> mAllocator;
    char *mMappedMemory = nullptr;

    void createBuffer(std::size_t size,
        const vk::BufferUsageFlags &usages);
    void allocateDeviceMemory(
        std::size_t size,
        const vk::MemoryPropertyFlags &mem_properties,
        const vk::BufferUsageFlags &usages);

public:
    VulkanMemoryPool(
        VulkanGpuDevice *device,
        std::size_t size,
        const vk::MemoryPropertyFlags &mem_properties,
        const vk::BufferUsageFlags &usages);
    ~VulkanMemoryPool();

    std::shared_ptr<VulkanBufferAllocation> allocate(std::size_t size);
    void deallocate(std::size_t offset);

    VulkanGpuDevice * device() const { return mDevice; }
    vk::DeviceMemory memory() const { return mMemory.get(); }
    vk::Buffer buffer() const { return mBuffer.get(); }
};
}
