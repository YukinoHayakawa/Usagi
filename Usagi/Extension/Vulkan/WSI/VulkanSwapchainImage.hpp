#pragma once

#include "../VulkanGpuImage.hpp"

namespace usagi
{
class VulkanSwapchainImage : public VulkanGpuImage
{
    // obtained from the presentation engine (image)
    vk::Image mImage;

public:
    VulkanSwapchainImage(
        vk::Device device,
        vk::Image image,
        vk::Format format,
        const Vector2u32 &size);

    vk::Image image() const override { return mImage; }

    // todo this should be a valid operation, but requires waiting on the image available semaphore
    void upload(const void *data, std::size_t size) override
    {
        throw std::runtime_error("Operation not supported.");
    }
};
}
