﻿#pragma once

#include "../VulkanGpuImage.hpp"

namespace usagi
{
class VulkanSwapchainImage : public VulkanGpuImage
{
    // obtained from the presentation engine (image)
    vk::Image mImage;

public:
    VulkanSwapchainImage(
        GpuImageFormat format,
        const Vector2u32 &size,
        vk::Device vk_device,
        vk::Image vk_image);

    vk::Image image() const override { return mImage; }

    // todo this should be a valid operation, but requires waiting on the image available semaphore
    void upload(const void *data, std::size_t size) override
    {
        throw std::runtime_error("Operation not supported.");
    }

    void uploadRegion(
        const void *buf_data,
        std::size_t buf_size,
        const Vector2i &tex_offset,
        const Vector2u32 &tex_size) override
    {
        throw std::runtime_error("Operation not supported.");
    }
};
}
