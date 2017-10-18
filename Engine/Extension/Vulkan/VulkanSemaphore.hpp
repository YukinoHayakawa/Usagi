#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/GraphicsSemaphore.hpp>

namespace yuki
{

class VulkanSemaphore : public GraphicsSemaphore
{
    vk::UniqueSemaphore mSemaphore;
    vk::PipelineStageFlags mSignalStage;

public:
    VulkanSemaphore() = default;
    VulkanSemaphore(vk::UniqueSemaphore semaphore, vk::PipelineStageFlags signal_stage);

    vk::Semaphore _getSemaphore() const;
    vk::PipelineStageFlags _getSignalStage() const;

    static std::vector<vk::Semaphore> _convertToVulkanHandles(const std::vector<GraphicsSemaphore*> &semaphores, std::vector<vk::PipelineStageFlags> *stages);
};

}
