#include "VulkanSemaphore.hpp"

yuki::VulkanSemaphore::VulkanSemaphore(vk::UniqueSemaphore semaphore, vk::PipelineStageFlagBits signal_stage)
    : mSemaphore { std::move(semaphore) }
    , mSignalStage { signal_stage }
{
}

uint64_t yuki::VulkanSemaphore::getSemaphoreHandle() const
{
    return reinterpret_cast<uint64_t>(VkSemaphore(mSemaphore.get()));
}

uint64_t yuki::VulkanSemaphore::getNativePipelineStage() const
{
    return static_cast<uint64_t>(mSignalStage);
}

vk::Semaphore yuki::VulkanSemaphore::_getSemaphore() const
{
    return mSemaphore.get();
}

vk::PipelineStageFlags yuki::VulkanSemaphore::_getSignalStage() const
{
    return { mSignalStage };
}
