#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/GpuCommandPool.hpp>

namespace usagi
{
class VulkanGpuDevice;

class VulkanGpuCommandPool : public GpuCommandPool
{
    VulkanGpuDevice *mDevice;
    vk::UniqueCommandPool mPool;

public:
    explicit VulkanGpuCommandPool(VulkanGpuDevice *device);

    std::shared_ptr<GraphicsCommandList> allocateGraphicsCommandList() override;
};
}
