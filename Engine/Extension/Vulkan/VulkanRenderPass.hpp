#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/RenderPass.hpp>
#include <Usagi/Engine/Runtime/Graphics/RenderPassCreateInfo.hpp>

namespace usagi
{
struct RenderPassCreateInfo;
class VulkanGpuDevice;

class VulkanRenderPass : public RenderPass
{
    vk::UniqueRenderPass mRenderPass;

public:
    VulkanRenderPass(VulkanGpuDevice *device, const RenderPassCreateInfo &info);

    vk::RenderPass renderPass() const
    {
        return mRenderPass.get();
    }
};
}
