#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/GraphicsSemaphore.hpp>

namespace yuki
{

class VulkanSemaphore : public GraphicsSemaphore
{
    vk::UniqueSemaphore mSemaphore;
    vk::PipelineStageFlagBits mSignalStage;

public:
    VulkanSemaphore() = default;
    VulkanSemaphore(vk::UniqueSemaphore semaphore, vk::PipelineStageFlagBits signal_stage);

    uint64_t getSemaphoreHandle() const override;
    uint64_t getNativePipelineStage() const override;

    vk::Semaphore _getSemaphore() const;
    vk::PipelineStageFlags _getSignalStage() const;
};

}
