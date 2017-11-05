#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Device/Waitable.hpp>

namespace yuki::vulkan
{

class Semaphore : public graphics::Waitable
{
    vk::UniqueSemaphore mSemaphore;
    vk::PipelineStageFlags mSignalStage;

public:
    Semaphore() = default;
    Semaphore(vk::UniqueSemaphore semaphore, vk::PipelineStageFlags signal_stage);

    vk::Semaphore _getSemaphore() const;
    vk::PipelineStageFlags _getSignalStage() const;

    static std::vector<vk::Semaphore> _convertToVulkanHandles(const std::vector<graphics::Waitable*> &semaphores, std::vector<vk::PipelineStageFlags> *stages);
};

}
