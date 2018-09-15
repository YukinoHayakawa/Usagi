#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Runtime/Graphics/RenderPass.hpp>

namespace usagi
{
struct RenderPassCreateInfo;
class VulkanGpuDevice;

class VulkanRenderPass : public RenderPass
{
    vk::UniqueRenderPass mRenderPass;
    std::vector<vk::ClearValue> mClearValues;

public:
    VulkanRenderPass(VulkanGpuDevice *device, const RenderPassCreateInfo &info);

    vk::RenderPass renderPass() const { return mRenderPass.get(); }

    const std::vector<vk::ClearValue> & clearValues() const
    {
        return mClearValues;
    }
};
}
