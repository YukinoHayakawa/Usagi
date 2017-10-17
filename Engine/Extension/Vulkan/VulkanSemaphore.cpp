#include "VulkanSemaphore.hpp"

yuki::VulkanSemaphore::VulkanSemaphore(vk::UniqueSemaphore semaphore, vk::PipelineStageFlags signal_stage)
    : mSemaphore { std::move(semaphore) }
    , mSignalStage { signal_stage }
{
}

vk::Semaphore yuki::VulkanSemaphore::_getSemaphore() const
{
    return mSemaphore.get();
}

vk::PipelineStageFlags yuki::VulkanSemaphore::_getSignalStage() const
{
    return mSignalStage;
}

std::vector<vk::Semaphore> yuki::VulkanSemaphore::_convertToVulkanHandles(
    const std::vector<GraphicsSemaphore *> &semaphores, std::vector<vk::PipelineStageFlags> *stages)
{
    std::vector<vk::Semaphore> vulkan_semaphores;
    for(auto &&s : semaphores)
    {
        const auto vulkan_semaphore = dynamic_cast<VulkanSemaphore*>(s);
        vulkan_semaphores.push_back(vulkan_semaphore->_getSemaphore());
        if(stages) stages->push_back(vulkan_semaphore->_getSignalStage());
    }
    return vulkan_semaphores;
}
