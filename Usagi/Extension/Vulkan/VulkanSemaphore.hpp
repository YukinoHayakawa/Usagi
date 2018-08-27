#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Runtime/Graphics/GpuSemaphore.hpp>

#include "VulkanBatchResource.hpp"

namespace usagi
{
class VulkanSemaphore
    : public GpuSemaphore
    , public VulkanBatchResource
{
    vk::UniqueSemaphore mSemaphore;

public:
    explicit VulkanSemaphore(vk::UniqueSemaphore vk_semaphore)
        : mSemaphore { std::move(vk_semaphore) }
    {
    }

    vk::Semaphore semaphore() const
    {
        return mSemaphore.get();
    }
};
}
