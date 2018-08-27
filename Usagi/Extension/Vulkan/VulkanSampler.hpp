#pragma once

#include <Usagi/Runtime/Graphics/GpuSampler.hpp>

#include "VulkanShaderResource.hpp"
#include "VulkanBatchResource.hpp"

namespace usagi
{
class VulkanSampler
    : public GpuSampler
    , public VulkanBatchResource
    , public VulkanShaderResource
{
    vk::UniqueSampler mSampler;

public:
    explicit VulkanSampler(vk::UniqueSampler vk_sampler);

    void fillShaderResourceInfo(
        vk::WriteDescriptorSet &write,
        VulkanResourceInfo &info) override;
};
}
