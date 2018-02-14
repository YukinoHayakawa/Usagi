#include "Semaphore.hpp"

namespace yuki::extension::vulkan
{

Semaphore::Semaphore(vk::UniqueSemaphore semaphore, vk::PipelineStageFlags signal_stage)
    : mSemaphore{ std::move(semaphore) }
    , mSignalStage{ signal_stage }
{
}

vk::Semaphore Semaphore::_getSemaphore() const
{
    return mSemaphore.get();
}

vk::PipelineStageFlags Semaphore::_getSignalStage() const
{
    return mSignalStage;
}

std::vector<vk::Semaphore> Semaphore::convertToVulkanHandles(
    const std::vector<Waitable *> &semaphores, std::vector<vk::PipelineStageFlags> *stages)
{
    std::vector<vk::Semaphore> vulkan_semaphores;
    for(auto &&s : semaphores)
    {
        const auto vulkan_semaphore = dynamic_cast<Semaphore*>(s);
        vulkan_semaphores.push_back(vulkan_semaphore->_getSemaphore());
        if(stages) stages->push_back(vulkan_semaphore->_getSignalStage());
    }
    return vulkan_semaphores;
}

}
