#include "VulkanMemoryPool.hpp"

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Runtime/Graphics/GpuImageCreateInfo.hpp>
#include <Usagi/Utility/Rounding.hpp>

#include "VulkanGpuDevice.hpp"
#include "VulkanEnumTranslation.hpp"

using namespace usagi::vulkan;

void usagi::VulkanMemoryPool::mapMemory()
{
    mMappedMemory = static_cast<char*>(mDevice->device().mapMemory(
        mMemory.get(), 0, VK_WHOLE_SIZE));
}

void usagi::VulkanMemoryPool::unmapMemory()
{
    if(mMappedMemory != nullptr)
    {
        mDevice->device().unmapMemory(mMemory.get());
        mMappedMemory = nullptr;
    }
}

void usagi::VulkanMemoryPool::allocateDeviceMemory(
    const vk::MemoryPropertyFlags &mem_properties)
{
    const auto memory_properties =
        mDevice->physicalDevice().getMemoryProperties();

    // find first heap supporting our needs and has enough space
    for(uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
    {
        // the i-th bit is set only when that memory type is supported.
        // https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkMemoryRequirements.html
        if((mMemoryRequirements.memoryTypeBits & 1 << i) &&
            (memory_properties.memoryTypes[i].propertyFlags & mem_properties))
        {
            vk::MemoryAllocateInfo info;
            info.setAllocationSize(mMemoryRequirements.size);
            info.setMemoryTypeIndex(i);
            try
            {
                mMemory = mDevice->device().allocateMemoryUnique(info);
                if(mem_properties & vk::MemoryPropertyFlagBits::eHostVisible)
                    mapMemory();
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

vk::UniqueImage usagi::VulkanMemoryPool::createImage(
    const GpuImageCreateInfo &info) const
{
    vk::ImageCreateInfo vk_info;
    vk_info.setImageType(vk::ImageType::e2D);
    vk_info.setFormat(translate(info.format));
    vk_info.extent.width = info.size.x();
    vk_info.extent.height = info.size.y();
    vk_info.extent.depth = 1;
    vk_info.setMipLevels(info.mip_levels);
    vk_info.setArrayLayers(1);
    vk_info.setSamples(translateSampleCount(info.sample_count));
    // todo optimal tiling support have to be checked using vkGetPhysicalDeviceImageFormatProperties before use
    vk_info.setTiling(vk::ImageTiling::eOptimal);
    vk_info.setUsage(
        translate(info.usage) |
        vk::ImageUsageFlagBits::eTransferDst);
    vk_info.setSharingMode(vk::SharingMode::eExclusive);
    vk_info.setInitialLayout(vk::ImageLayout::eUndefined);

    return mDevice->device().createImageUnique(vk_info);
}

vk::MemoryRequirements usagi::VulkanMemoryPool::getImageRequirements(
    const vk::Image image) const
{
    return mDevice->device().getImageMemoryRequirements(image);
}

void usagi::VulkanMemoryPool::bindImageMemory(
    VulkanPooledImage *image)
{
    mDevice->device().bindImageMemory(image->image(), mMemory.get(),
        image->offset());
}

void usagi::VulkanMemoryPool::createImageBaseView(VulkanPooledImage *image)
{
    image->createBaseView();
}

void usagi::VulkanMemoryPool::allocateDeviceMemoryForBuffer(
    const std::size_t size,
    const vk::MemoryPropertyFlags &mem_properties,
    const vk::BufferUsageFlags &usages,
    vk::UniqueBuffer &buffer)
{
    vk::BufferCreateInfo buffer_create_info;

    buffer_create_info.setSize(size);
    buffer_create_info.setUsage(usages);
    buffer_create_info.setSharingMode(vk::SharingMode::eExclusive);

    auto vk_device = mDevice->device();
    buffer = vk_device.createBufferUnique(buffer_create_info);

    mMemoryRequirements = vk_device.getBufferMemoryRequirements(buffer.get());
    allocateDeviceMemory(mem_properties);
    vk_device.bindBufferMemory(buffer.get(), mMemory.get(), 0);
}

void usagi::VulkanMemoryPool::allocateDeviceMemoryForImage(
    std::size_t total_size,
    const vk::MemoryPropertyFlags &mem_properties,
    const vk::ImageUsageFlags &usages)
{
    vk::ImageCreateInfo vk_info;
    vk_info.setImageType(vk::ImageType::e2D);
    vk_info.setFormat(vk::Format::eR32G32B32A32Sfloat);
    vk_info.extent.width = 2048;
    vk_info.extent.height = 2048;
    vk_info.extent.depth = 1;
    vk_info.setMipLevels(1);
    vk_info.setArrayLayers(1);
    vk_info.setSamples(vk::SampleCountFlagBits::e1);
    vk_info.setTiling(vk::ImageTiling::eOptimal);
    vk_info.setUsage(usages);
    vk_info.setSharingMode(vk::SharingMode::eExclusive);
    vk_info.setInitialLayout(vk::ImageLayout::eUndefined);

    auto vk_device = mDevice->device();
    auto dummy_image = vk_device.createImageUnique(vk_info);

    mMemoryRequirements =
        vk_device.getImageMemoryRequirements(dummy_image.get());
    mMemoryRequirements.size =
        utility::roundUpUnsigned(total_size, mMemoryRequirements.alignment);

    allocateDeviceMemory(mem_properties);
}

usagi::VulkanMemoryPool::VulkanMemoryPool(VulkanGpuDevice *device)
    : mDevice(device)
{
}

usagi::VulkanMemoryPool::~VulkanMemoryPool()
{
    unmapMemory();
}
