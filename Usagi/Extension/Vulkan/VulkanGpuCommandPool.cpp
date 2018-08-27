#include "VulkanGpuCommandPool.hpp"

#include "VulkanGpuDevice.hpp"
#include "VulkanGraphicsCommandList.hpp"

usagi::VulkanGpuCommandPool::VulkanGpuCommandPool(VulkanGpuDevice *device)
    : mDevice { device }
{
    vk::CommandPoolCreateInfo info;

    info.setQueueFamilyIndex(mDevice->graphicsQueueFamily());
    // our command lists are freed immediately after each frame.
    info.setFlags(vk::CommandPoolCreateFlagBits::eTransient);

    mPool = mDevice->device().createCommandPoolUnique(info);
}

std::shared_ptr<usagi::GraphicsCommandList> usagi::VulkanGpuCommandPool::
    allocateGraphicsCommandList()
{
    vk::CommandBufferAllocateInfo info;

    info.setCommandBufferCount(1);
    info.setCommandPool(mPool.get());
    info.setLevel(vk::CommandBufferLevel::ePrimary);

    return std::make_shared<VulkanGraphicsCommandList>(
        shared_from_this(),
        std::move(mDevice->device().allocateCommandBuffersUnique(info).front())
    );
}
