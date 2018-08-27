#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Runtime/Graphics/Swapchain.hpp>

#include "VulkanSwapchainImage.hpp"

namespace usagi
{
class VulkanGpuDevice;

class VulkanSwapchain final : public Swapchain
{
    VulkanGpuDevice *mDevice;

    vk::UniqueSurfaceKHR mSurface;
    vk::SurfaceFormatKHR mFormat;
    Vector2u32 mSize;

    vk::UniqueSwapchainKHR mSwapchain;

    static inline constexpr uint32_t INVALID_IMAGE_INDEX = -1;
    uint32_t mCurrentImageIndex = INVALID_IMAGE_INDEX;
    std::vector<std::shared_ptr<VulkanSwapchainImage>> mSwapchainImages;
    int mImagesInUse = 0;

    static vk::SurfaceFormatKHR selectSurfaceFormat(
        const std::vector<vk::SurfaceFormatKHR> &surface_formats,
        vk::Format preferred_image_format);
    vk::Extent2D selectSurfaceExtent(
        const Vector2u32 &size,
        const vk::SurfaceCapabilitiesKHR &surface_capabilities) const;
    static vk::PresentModeKHR selectPresentMode(
        const std::vector<vk::PresentModeKHR> &present_modes);
    std::uint32_t selectPresentationQueueFamily() const;

    void createSwapchain(const Vector2u32 &size, vk::Format image_format);
    void getSwapchainImages();

public:
    VulkanSwapchain(
        VulkanGpuDevice *device,
        vk::UniqueSurfaceKHR vk_surface_khr);

    void create(const Vector2u32 &size, GpuBufferFormat format) override;
    void resize(const Vector2u32 &size) override;

    GpuBufferFormat format() const override;
    Vector2u32 size() const override;

    std::shared_ptr<GpuSemaphore> acquireNextImage() override;
    GpuImage * currentImage() override;

    void present(std::initializer_list<std::shared_ptr<GpuSemaphore>>
        wait_semaphores) override;

    void present(const std::vector<vk::Semaphore> &wait_semaphores);
};
}

