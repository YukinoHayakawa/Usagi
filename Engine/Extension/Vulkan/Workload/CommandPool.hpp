#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Workload/CommandPool.hpp>

namespace yuki::vulkan
{

class CommandPool : public graphics::CommandPool
{
    class Device *mVulkanGD;
    vk::UniqueCommandPool mCommandPool;

public:
    CommandPool(Device *vulkan_gd);

    std::unique_ptr<graphics::CommandList> createCommandList() override;
};

}
