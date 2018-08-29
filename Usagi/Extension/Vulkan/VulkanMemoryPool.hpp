#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Utility/Noncopyable.hpp>
#include <Usagi/Runtime/Graphics/GpuImageCreateInfo.hpp>

#include "VulkanBufferAllocation.hpp"
#include "VulkanPooledImage.hpp"

namespace usagi
{
class GpuCommandPool;
struct GpuImageCreateInfo;
class VulkanGpuDevice;
class VulkanBufferAllocation;

class VulkanMemoryPool : Noncopyable
{
protected:
    VulkanGpuDevice *mDevice = nullptr;
    vk::UniqueDeviceMemory mMemory;
    vk::MemoryRequirements mMemoryRequirements { };
    // the memory will be automatically mapped if has eHostVisible flag
    char *mMappedMemory = nullptr;

    void mapMemory();
    void unmapMemory();

    void allocateDeviceMemory(const vk::MemoryPropertyFlags &mem_properties);

    vk::UniqueImage createImage(
        const GpuImageCreateInfo &info, vk::Format &format) const;
    vk::MemoryRequirements getImageRequirements(vk::Image image) const;
    void bindImageMemory(VulkanPooledImage *image);
    static void createImageBaseView(VulkanPooledImage *image);

    void allocateDeviceMemoryForBuffer(
        std::size_t size,
        const vk::MemoryPropertyFlags &mem_properties,
        const vk::BufferUsageFlags &usages,
        vk::UniqueBuffer &buffer);

    void allocateDeviceMemoryForImage(
        std::size_t total_size,
        const vk::MemoryPropertyFlags &mem_properties,
        const vk::ImageUsageFlags &usages);

public:
    VulkanMemoryPool(VulkanGpuDevice *device);
    virtual ~VulkanMemoryPool();

    virtual void deallocate(std::size_t offset) = 0;

    VulkanGpuDevice * device() const { return mDevice; }
    vk::DeviceMemory memory() const { return mMemory.get(); }
};

class VulkanBufferMemoryPoolBase : public VulkanMemoryPool
{
protected:
    vk::UniqueBuffer mBuffer;

public:
    using VulkanMemoryPool::VulkanMemoryPool;

    virtual std::shared_ptr<VulkanBufferAllocation> allocate(
        std::size_t size) = 0;

    vk::Buffer buffer() const { return mBuffer.get(); }
};

template <typename Allocator>
class VulkanBufferMemoryPool : public VulkanBufferMemoryPoolBase
{
    std::unique_ptr<Allocator> mAllocator;

public:
    template <typename AllocCreateFunc>
    VulkanBufferMemoryPool(
        VulkanGpuDevice *device,
        const std::size_t size,
        const vk::MemoryPropertyFlags &mem_properties,
        const vk::BufferUsageFlags &usages,
        AllocCreateFunc alloc_create_func)
        : VulkanBufferMemoryPoolBase(device)
    {
        allocateDeviceMemoryForBuffer(size, mem_properties, usages, mBuffer);
        mAllocator = alloc_create_func(mMemoryRequirements);
    }

    ~VulkanBufferMemoryPool()
    {
        // all memory all freed
        assert(mAllocator || mAllocator->usedSize() == 0);
    }

    std::shared_ptr<VulkanBufferAllocation> allocate(std::size_t size) override
    {
        auto offset = reinterpret_cast<std::size_t>(mAllocator->allocate(size));
        try
        {
            auto alloc = std::make_shared<VulkanBufferAllocation>(
                this,
                offset, size,
                mMappedMemory ? mMappedMemory + offset : nullptr
            );
            return std::move(alloc);
        }
        catch(...)
        {
            deallocate(offset);
            throw;
        }
    }

    void deallocate(const std::size_t offset) override
    {
        mAllocator->deallocate(reinterpret_cast<void*>(offset));
    }
};

template <typename Allocator>
class VulkanImageMemoryPool : public VulkanMemoryPool
{
    std::unique_ptr<Allocator> mAllocator;

public:
    template <typename AllocCreateFunc>
    VulkanImageMemoryPool(
        VulkanGpuDevice *device,
        std::size_t size,
        const vk::MemoryPropertyFlags &mem_properties,
        const vk::ImageUsageFlags &usages,
        AllocCreateFunc alloc_create_func)
        : VulkanMemoryPool(device)
    {
        allocateDeviceMemoryForImage(size, mem_properties, usages);
        mAllocator = alloc_create_func(mMemoryRequirements);
    }

    ~VulkanImageMemoryPool()
    {
        // all memory all freed
        assert(mAllocator || mAllocator->usedSize() == 0);
    }

    std::shared_ptr<VulkanPooledImage> createPooledImage(
        const GpuImageCreateInfo &info)
    {
        vk::Format vk_format;
        auto image = createImage(info, vk_format);
        const auto req = getImageRequirements(image.get());
        auto offset = reinterpret_cast<std::size_t>(mAllocator->allocate(
            req.size, req.alignment));
        try
        {
            auto wrapper = std::make_shared<VulkanPooledImage>(
                std::move(image), vk_format, info.size,
                this, offset, req.size
            );
            bindImageMemory(wrapper.get());
            createImageBaseView(wrapper.get());
            return std::move(wrapper);
        }
        catch(...)
        {
            deallocate(offset);
            throw;
        }
    }

    void deallocate(const std::size_t offset) override
    {
        mAllocator->deallocate(reinterpret_cast<void*>(offset));
    }
};
}
