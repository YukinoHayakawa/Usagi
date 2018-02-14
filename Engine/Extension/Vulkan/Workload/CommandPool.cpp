#include "../Device/Device.hpp"
#include "CommandPool.hpp"
#include "CommandList.hpp"

namespace yuki::extension::vulkan
{

CommandPool::CommandPool(Device *vulkan_gd)
    : mVulkanGD { vulkan_gd }
{
    vk::CommandPoolCreateInfo pool_create_info;
    pool_create_info.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient);
    pool_create_info.setQueueFamilyIndex(mVulkanGD->getGraphicsQueueFamilyIndex());

    mCommandPool = mVulkanGD->device().createCommandPoolUnique(pool_create_info);
}

std::unique_ptr<graphics::CommandList> CommandPool::createCommandList()
{
    vk::CommandBufferAllocateInfo command_buffer_allocate_info;
    command_buffer_allocate_info.setCommandPool(mCommandPool.get());
    command_buffer_allocate_info.setCommandBufferCount(1);
    command_buffer_allocate_info.setLevel(vk::CommandBufferLevel::ePrimary);

    auto buffer = std::move(mVulkanGD->device().allocateCommandBuffersUnique(command_buffer_allocate_info)[0]);
    auto command_list = std::make_unique<CommandList>(mVulkanGD, std::move(buffer));

    return std::move(command_list);
}

}
