#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/GraphicsCommandPool.hpp>

namespace yuki
{

class VulkanGraphicsCommandPool : public GraphicsCommandPool
{
    class VulkanGraphicsDevice *mVulkanGD;
    vk::UniqueCommandPool mCommandPool;

public:
    VulkanGraphicsCommandPool(VulkanGraphicsDevice *vulkan_gd);

    std::unique_ptr<GraphicsCommandList> createCommandList() override;
};

}
