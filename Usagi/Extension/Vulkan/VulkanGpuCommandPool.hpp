#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Runtime/Graphics/Resource/GpuCommandPool.hpp>

namespace usagi
{
class VulkanGpuDevice;

class VulkanGpuCommandPool
    : public GpuCommandPool
    , public std::enable_shared_from_this<VulkanGpuCommandPool>
{
    VulkanGpuDevice *mDevice;
    vk::UniqueCommandPool mPool;

public:
    explicit VulkanGpuCommandPool(VulkanGpuDevice *device);

    std::shared_ptr<GraphicsCommandList> allocateGraphicsCommandList() override;

    VulkanGpuDevice * device() const { return mDevice; }
};
}
