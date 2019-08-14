#include "VulkanSwapchain.hpp"

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Runtime/Window/Window.hpp>
#include <Usagi/Utility/TypeCast.hpp>

#include "../VulkanGpuDevice.hpp"
#include "../VulkanEnumTranslation.hpp"
#include "../VulkanHelper.hpp"
#include "../VulkanSemaphore.hpp"

using namespace usagi::vulkan;

usagi::VulkanSwapchain::VulkanSwapchain(
    VulkanGpuDevice *device,
    vk::UniqueSurfaceKHR vk_surface_khr)
    : mDevice { device }
    , mSurface { std::move(vk_surface_khr) }
    , mFormat { vk::Format::eUndefined, vk::ColorSpaceKHR::eSrgbNonlinear }
{
}

std::shared_ptr<usagi::GpuSemaphore> usagi::VulkanSwapchain::acquireNextImage()
{
    if(mImagesInUse != 0)
        LOG(warn, "Requesting another image from swapchain while "
            "already {} is used.", mImagesInUse);

    auto sem = mDevice->createSemaphore();
    const auto vk_sem = static_cast<VulkanSemaphore&>(*sem).semaphore();

    // todo sometimes hangs when debugging with RenderDoc
    const auto result = mDevice->device().acquireNextImageKHR(
        mSwapchain.get(),
        UINT64_MAX, // 1000000000u, // 1s
        vk_sem,
        nullptr,
        &mCurrentImageIndex
    );
    switch(result)
    {
        case vk::Result::eSuboptimalKHR:
            LOG(warn, "Suboptimal swapchain");
        case vk::Result::eSuccess: break;
        case vk::Result::eNotReady:
            USAGI_THROW(std::runtime_error("Currently no usable swapchain image."));
        case vk::Result::eTimeout:
            USAGI_THROW(std::runtime_error("acquireNextImageKHR() timed out."));

        case vk::Result::eErrorOutOfDateKHR:
            LOG(info, "Swapchain is out-of-date, recreating");
            createSwapchain(mSize, mFormat.format);
            return acquireNextImage();

        default: USAGI_THROW(std::runtime_error("acquireNextImageKHR() failed."));
    }

    ++mImagesInUse;
    return sem;
}

usagi::GpuImage * usagi::VulkanSwapchain::currentImage()
{
    return mSwapchainImages[mCurrentImageIndex].get();
}

void usagi::VulkanSwapchain::present(
    std::initializer_list<std::shared_ptr<GpuSemaphore>> wait_semaphores)
{
    const auto sems = vulkan::transformObjects(wait_semaphores,
        [&](auto &&s) {
            return dynamic_cast_ref<VulkanSemaphore>(s).semaphore();
        }
    );
    present(sems);
    --mImagesInUse;
}

void usagi::VulkanSwapchain::present(
    const std::vector<vk::Semaphore> &wait_semaphores)
{
    vk::PresentInfoKHR info;

    info.setWaitSemaphoreCount(static_cast<uint32_t>(wait_semaphores.size()));
    info.setPWaitSemaphores(wait_semaphores.data());
    const auto sc_handle = mSwapchain.get();
    info.setSwapchainCount(1);
    info.setPSwapchains(&sc_handle);
    info.setPImageIndices(&mCurrentImageIndex);

    // https://github.com/KhronosGroup/Vulkan-Docs/issues/595
    // Seems that after the semaphores waited on have all signaled,
    // the presentation will be finished almost immediately, so
    // tracking the reference of the semaphores is not necessary (if it is
    // possible).

    // If the swapchain is suboptimal or out-of-date, it will be recreated
    // during next call of acquireNextImage().
    switch(mDevice->presentQueue().presentKHR(&info))
    {
        case vk::Result::eSuccess: break;
        case vk::Result::eSuboptimalKHR:
            LOG(warn, "Presented to a suboptimal swapchain.");
            break;
        case vk::Result::eErrorOutOfDateKHR:
            LOG(error, "Presented to a out-of-date swapchain.");
            break;
        default: ;
    }
}

// todo: allow choosing colorspace
vk::SurfaceFormatKHR usagi::VulkanSwapchain::selectSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR> &surface_formats,
    const vk::Format preferred_image_format)
{
    // If the list contains only one entry with undefined format
    // it means that there are no preferred surface formats and any can be
    // chosen
    if(surface_formats.size() == 1 &&
        surface_formats[0].format == vk::Format::eUndefined)
        return {
            preferred_image_format,
            vk::ColorSpaceKHR::eSrgbNonlinear
        };

    // todo: fallback
    for(auto &&surface_format : surface_formats)
    {
        if(surface_format.format == preferred_image_format)
        {
            return surface_format;
        }
    }

    // bug: NVIDIA card has only limited color formats
    // Return the first format from the list
    return surface_formats[0];
}

vk::Extent2D usagi::VulkanSwapchain::selectSurfaceExtent(
    const Vector2u32 &size,
    const vk::SurfaceCapabilitiesKHR &surface_capabilities) const
{
    // currentExtent is the current width and height of the surface, or
    // the special value (0xFFFFFFFF, 0xFFFFFFFF) indicating that the
    // surface size will be determined by the extent of a swapchain
    // targeting the surface.
    if(surface_capabilities.currentExtent.width == 0xFFFFFFFF)
    {
        vk::Extent2D swap_chain_extent = { size.x(), size.y() };
        swap_chain_extent.width        = std::clamp(swap_chain_extent.width,
            surface_capabilities.minImageExtent.width,
            surface_capabilities.maxImageExtent.width
        );
        swap_chain_extent.height = std::clamp(swap_chain_extent.height,
            surface_capabilities.minImageExtent.height,
            surface_capabilities.maxImageExtent.height
        );
        return swap_chain_extent;
    }
    // Most of the cases we define size of the swap_chain images equal to
    // current window's size
    return surface_capabilities.currentExtent;
}

vk::PresentModeKHR usagi::VulkanSwapchain::selectPresentMode(
    const std::vector<vk::PresentModeKHR> &present_modes)
{
    // todo: allow disable v-sync
    // prefer mailbox mode to achieve triple buffering
    if(std::find(present_modes.begin(), present_modes.end(),
        vk::PresentModeKHR::eMailbox) != present_modes.end())
        return vk::PresentModeKHR::eMailbox;
    return vk::PresentModeKHR::eFifo;
}

std::uint32_t usagi::VulkanSwapchain::selectPresentationQueueFamily() const
{
    // todo use vkGetPhysicalDeviceWin32PresentationSupportKHR
    const auto queue_families =
        mDevice->physicalDevice().getQueueFamilyProperties();
    for(auto i = queue_families.begin(); i != queue_families.end(); ++i)
    {
        const auto queue_index =
            static_cast<uint32_t>(i - queue_families.begin());
        if(mDevice->physicalDevice().getSurfaceSupportKHR(
            queue_index, mSurface.get()))
            return queue_index;
    }
    USAGI_THROW(std::runtime_error("No queue family supporting WSI was found."));
}

void usagi::VulkanSwapchain::create(
    const Vector2u32 &size,
    GpuBufferFormat format)
{
    createSwapchain(size, translate(format));
}

void usagi::VulkanSwapchain::resize(const Vector2u32 &size)
{
    if(mSize != size)
        createSwapchain(size, mFormat.format);
}

usagi::GpuBufferFormat usagi::VulkanSwapchain::format() const
{
    return translate(mFormat.format);
}

usagi::Vector2u32 usagi::VulkanSwapchain::size() const
{
    return mSize;
}

void usagi::VulkanSwapchain::createSwapchain(
    const Vector2u32 &size,
    vk::Format image_format)
{
    assert(size.x() > 0 && size.y() > 0);

    // Ensure that no operation involving the swapchain images is outstanding.
    // Since acquireNextImage() and drawing operations aren't parallel,
    // as long as the device is idle, it won't happen.
    mDevice->device().waitIdle();

    LOG(info, "Creating swapchain");

    // todo: query using vkGetPhysicalDeviceWin32PresentationSupportKHR
    // mPresentationQueueFamilyIndex = selectPresentationQueueFamily();
    // LOG(info, "Using queue family {} for presentation",
    //     mPresentationQueueFamilyIndex);

    const auto surface_capabilities =
        mDevice->physicalDevice().getSurfaceCapabilitiesKHR(mSurface.get());
    const auto surface_formats =
        mDevice->physicalDevice().getSurfaceFormatsKHR(mSurface.get());
    const auto surface_present_modes =
        mDevice->physicalDevice().getSurfacePresentModesKHR(mSurface.get());

    vk::SwapchainCreateInfoKHR create_info;

    create_info.setSurface(mSurface.get());
    const auto vk_format = selectSurfaceFormat(surface_formats, image_format);
    LOG(info, "Surface format: {}", to_string(vk_format.format));
    LOG(info, "Surface colorspace: {}", to_string(vk_format.colorSpace));
    create_info.setImageFormat(vk_format.format);
    create_info.setImageColorSpace(vk_format.colorSpace);
    create_info.setImageExtent(selectSurfaceExtent(size, surface_capabilities));
    LOG(info, "Surface extent: {}x{}",
        create_info.imageExtent.width, create_info.imageExtent.height);

    create_info.setPresentMode(selectPresentMode(surface_present_modes));
    // todo: mailbox not available on my R9 290X
    // todo triple buffering
    if(create_info.presentMode == vk::PresentModeKHR::eMailbox)
        // Ensures non-blocking vkAcquireNextImageKHR() in mailbox mode.
        // See 3.6.12 of http://vulkan-spec-chunked.ahcox.com/apes03.html
        create_info.setMinImageCount(surface_capabilities.minImageCount + 1);
    else
        create_info.setMinImageCount(surface_capabilities.minImageCount);

    create_info.setImageArrayLayers(1);
    create_info.setImageSharingMode(vk::SharingMode::eExclusive);
    create_info.setPreTransform(surface_capabilities.currentTransform);

    create_info.setImageUsage(
        vk::ImageUsageFlagBits::eColorAttachment |
        vk::ImageUsageFlagBits::eTransferDst
    );

    create_info.setOldSwapchain(mSwapchain.get());

    mSwapchain = mDevice->device().createSwapchainKHRUnique(create_info);
    mFormat = vk_format;
    mSize = { create_info.imageExtent.width, create_info.imageExtent.height };

    getSwapchainImages();
}

void usagi::VulkanSwapchain::getSwapchainImages()
{
    auto images = mDevice->device().getSwapchainImagesKHR(mSwapchain.get());

    mSwapchainImages.clear();
    mSwapchainImages.reserve(images.size());
    const GpuImageFormat format { translate(mFormat.format), 1 };
    for(auto &&vk_image : images)
    {
        mSwapchainImages.push_back(std::make_shared<VulkanSwapchainImage>(
            format, mSize, mDevice->device(), vk_image));
    }
    mCurrentImageIndex = INVALID_IMAGE_INDEX;
}
