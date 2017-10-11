#include "VulkanGraphicsCommandPool.hpp"
#include "VulkanGraphicsDevice.hpp"
#include "VulkanGraphicsCommandList.hpp"

yuki::VulkanGraphicsCommandPool::VulkanGraphicsCommandPool(VulkanGraphicsDevice *vulkan_gd)
    : mVulkanGD { vulkan_gd }
{
    vk::CommandPoolCreateInfo pool_create_info;
    pool_create_info.setQueueFamilyIndex(mVulkanGD->getGraphicsQueueFamilyIndex());

    mCommandPool = mVulkanGD->_getDevice().createCommandPoolUnique(pool_create_info);
}

std::unique_ptr<yuki::GraphicsCommandList> yuki::VulkanGraphicsCommandPool::createCommandList()
{
    vk::CommandBufferAllocateInfo command_buffer_allocate_info;
    command_buffer_allocate_info.setCommandPool(mCommandPool.get());
    command_buffer_allocate_info.setCommandBufferCount(1);
    command_buffer_allocate_info.setLevel(vk::CommandBufferLevel::ePrimary);

    auto buffer = std::move(mVulkanGD->_getDevice().allocateCommandBuffersUnique(command_buffer_allocate_info)[0]);
    auto command_list = std::make_unique<VulkanGraphicsCommandList>(mVulkanGD, std::move(buffer));

    return std::move(command_list);
}
