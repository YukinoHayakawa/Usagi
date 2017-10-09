#include <Usagi/Engine/Utility/Math.hpp>
#include <Usagi/Engine/Runtime/Exception.hpp>

#include "VulkanSwapChain.hpp"
#include "VulkanGraphicsDevice.hpp"

vk::SurfaceFormatKHR yuki::VulkanSwapChain::_selectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &surface_formats)
{
    // If the list contains only one entry with undefined format
    // it means that there are no preferred surface formats and any can be chosen
    if(surface_formats.size() == 1 && surface_formats[0].format == vk::Format::eUndefined)
    {
        return { vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
    }

    // Check if list contains most widely used R8 G8 B8 A8 format
    // with nonlinear color space
    for(auto &&surface_format : surface_formats)
    {
        if(surface_format.format == vk::Format::eR8G8B8A8Unorm)
        {
            return surface_format;
        }
    }

    // Return the first format from the list
    return surface_formats[0];
}

vk::Extent2D yuki::VulkanSwapChain::_selectSurfaceExtent(const vk::SurfaceCapabilitiesKHR &surface_capabilities)
{
    // Special value of surface extent is width == height == -1
    // If this is so we define the size by ourselves but it must fit within defined confines
    if(surface_capabilities.currentExtent.width == -1)
    {
        vk::Extent2D swap_chain_extent = { 1280, 720 };
        swap_chain_extent.width = clamp(swap_chain_extent.width, { surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width });
        swap_chain_extent.height = clamp(swap_chain_extent.height, { surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height });
        return swap_chain_extent;
    }
    // Most of the cases we define size of the swap_chain images equal to current window's size
    return surface_capabilities.currentExtent;
}

vk::PresentModeKHR yuki::VulkanSwapChain::_selectPresentMode(const std::vector<vk::PresentModeKHR> &present_modes)
{
    // prefer mailbox mode to achieve triple buffering
    if(find(present_modes.begin(), present_modes.end(), vk::PresentModeKHR::eMailbox) != present_modes.end())
        return vk::PresentModeKHR::eMailbox;
    return vk::PresentModeKHR::eFifo;
}

uint32_t yuki::VulkanSwapChain::_selectPresentationQueueFamily() const
{
    const auto queue_families = mVulkanGD->mPhysicalDevice.getQueueFamilyProperties();
    for(auto iter = queue_families.begin(); iter != queue_families.end(); ++iter)
    {
        const uint32_t queue_index = iter - queue_families.begin();
        if(mVulkanGD->mPhysicalDevice.getSurfaceSupportKHR(queue_index, mSurface.get()))
            return queue_index;
    }
    throw GraphicsAPIUnsupportedFeatureException() << FeatureDescriptionInfo("no queue family supporting WSI was found");
}

// todo recreate after resizing the window
void yuki::VulkanSwapChain::_createSwapChain()
{
    mPresentationQueueFamilyIndex = _selectPresentationQueueFamily();

    const auto surface_capabilities = mVulkanGD->mPhysicalDevice.getSurfaceCapabilitiesKHR(mSurface.get());
    const auto surface_formats = mVulkanGD->mPhysicalDevice.getSurfaceFormatsKHR(mSurface.get());
    const auto surface_present_modes = mVulkanGD->mPhysicalDevice.getSurfacePresentModesKHR(mSurface.get());

    vk::SwapchainCreateInfoKHR swapchain_create_info_khr;

    swapchain_create_info_khr.setSurface(mSurface.get());
    const auto format = _selectSurfaceFormat(surface_formats);
    mSurfaceFormat = format.format;
    swapchain_create_info_khr.setImageFormat(format.format);
    swapchain_create_info_khr.setImageColorSpace(format.colorSpace);
    swapchain_create_info_khr.setImageExtent(_selectSurfaceExtent(surface_capabilities));

    // set up triple buffering
    const uint32_t image_count = 3;
    if(!withinOpenInterval(image_count, { surface_capabilities.minImageCount, surface_capabilities.maxImageCount }))
        throw GraphicsAPIUnsupportedFeatureException() << FeatureDescriptionInfo("the device does not support triple buffering");
    swapchain_create_info_khr.setMinImageCount(image_count);
    swapchain_create_info_khr.setPresentMode(_selectPresentMode(surface_present_modes));

    swapchain_create_info_khr.setImageArrayLayers(1);
    swapchain_create_info_khr.setImageSharingMode(vk::SharingMode::eExclusive);
    swapchain_create_info_khr.setPreTransform(surface_capabilities.currentTransform);

    swapchain_create_info_khr.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst);

    swapchain_create_info_khr.setOldSwapchain(mSwapChain.get());

    mSwapChain = mVulkanGD->mDevice.createSwapchainKHRUnique(swapchain_create_info_khr);
}

void yuki::VulkanSwapChain::_createCommandBuffers()
{
    vk::CommandPoolCreateInfo cmd_pool_create_info;
    cmd_pool_create_info.setQueueFamilyIndex(mPresentationQueueFamilyIndex);
    mPresentCommandPool = mVulkanGD->mDevice.createCommandPoolUnique(cmd_pool_create_info);

    mSwapChainImages = mVulkanGD->mDevice.getSwapchainImagesKHR(mSwapChain.get());

    vk::CommandBufferAllocateInfo cmd_buffer_allocate_info;
    cmd_buffer_allocate_info.setCommandPool(mPresentCommandPool.get());
    cmd_buffer_allocate_info.setLevel(vk::CommandBufferLevel::ePrimary);
    cmd_buffer_allocate_info.setCommandBufferCount(mSwapChainImages.size());
    mPresentCommandBuffers = mVulkanGD->mDevice.allocateCommandBuffersUnique(cmd_buffer_allocate_info);
}

void yuki::VulkanSwapChain::_recordCommands()
{
    vk::CommandBufferBeginInfo cmd_buffer_begin_info;
    cmd_buffer_begin_info.setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

    vk::ClearColorValue clear_color;
    clear_color.setFloat32({ 1.0f, 0.8f, 0.4f, 0.0f });

    vk::ImageSubresourceRange image_subresource_range;
    image_subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);
    image_subresource_range.setBaseMipLevel(0);
    image_subresource_range.setLevelCount(1);
    image_subresource_range.setBaseArrayLayer(0);
    image_subresource_range.setLayerCount(1);

    const uint32_t image_count = static_cast<uint32_t>(mSwapChainImages.size());
    for(uint32_t i = 0; i < image_count; ++i)
    {
        vk::ImageMemoryBarrier barrier_from_present_to_clear;
        barrier_from_present_to_clear.setSrcAccessMask({ });
        barrier_from_present_to_clear.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier_from_present_to_clear.setOldLayout(vk::ImageLayout::eUndefined); // discard old content
        barrier_from_present_to_clear.setNewLayout(vk::ImageLayout::eTransferDstOptimal);
        barrier_from_present_to_clear.setSrcQueueFamilyIndex(mPresentationQueueFamilyIndex); // ?
        barrier_from_present_to_clear.setDstQueueFamilyIndex(mPresentationQueueFamilyIndex);
        barrier_from_present_to_clear.setImage(mSwapChainImages[i]);
        barrier_from_present_to_clear.setSubresourceRange(image_subresource_range);

        vk::ImageMemoryBarrier barrier_from_clear_to_present;
        barrier_from_clear_to_present.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier_from_clear_to_present.setDstAccessMask(vk::AccessFlagBits::eMemoryRead);
        barrier_from_clear_to_present.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
        barrier_from_clear_to_present.setNewLayout(vk::ImageLayout::ePresentSrcKHR);
        barrier_from_clear_to_present.setSrcQueueFamilyIndex(mPresentationQueueFamilyIndex);
        barrier_from_clear_to_present.setDstQueueFamilyIndex(mPresentationQueueFamilyIndex);
        barrier_from_clear_to_present.setImage(mSwapChainImages[i]);
        barrier_from_clear_to_present.setSubresourceRange(image_subresource_range); 

        mPresentCommandBuffers[i]->begin(cmd_buffer_begin_info);
        // todo: understand the semantic
        mPresentCommandBuffers[i]->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, { }, nullptr, nullptr, { barrier_from_present_to_clear });
        // todo: the operation should be posted to graphics command queue
        mPresentCommandBuffers[i]->clearColorImage(mSwapChainImages[i], vk::ImageLayout::eTransferDstOptimal, clear_color, { image_subresource_range });
        mPresentCommandBuffers[i]->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe, { }, nullptr, nullptr, { barrier_from_clear_to_present });
        mPresentCommandBuffers[i]->end();
    }
}

yuki::VulkanSwapChain::VulkanSwapChain(std::shared_ptr<VulkanGraphicsDevice> device, HINSTANCE hInstance, HWND hWnd)
    : mVulkanGD(std::move(device))
{
    vk::SemaphoreCreateInfo semaphore_create_info;
    mImageAvailableSemaphore = mVulkanGD->mDevice.createSemaphoreUnique(semaphore_create_info);
    mRenderingFinishedSemaphore = mVulkanGD->mDevice.createSemaphoreUnique(semaphore_create_info);

    vk::Win32SurfaceCreateInfoKHR surface_create_info;
    surface_create_info.setHinstance(hInstance);
    surface_create_info.setHwnd(hWnd);
    mSurface = mVulkanGD->mInstance.createWin32SurfaceKHRUnique(surface_create_info);

    _createSwapChain();
    _createCommandBuffers();
    _recordCommands();
}

yuki::VulkanSwapChain::~VulkanSwapChain()
{
}

void yuki::VulkanSwapChain::present()
{
    const auto result = mVulkanGD->mDevice.acquireNextImageKHR(
        mSwapChain.get(),
        100000000, // 100ms
        mImageAvailableSemaphore.get(), { }
    );
    switch(result.result)
    {
        case vk::Result::eSuccess: break;
        case vk::Result::eNotReady:
        case vk::Result::eTimeout:
        case vk::Result::eSuboptimalKHR:
        default:
            throw;
    }

    vk::PipelineStageFlags wait_dst_stage_mask = vk::PipelineStageFlagBits::eTransfer;
    vk::SubmitInfo submit_info;
    submit_info.setWaitSemaphoreCount(1);
    submit_info.setPWaitSemaphores(&*mImageAvailableSemaphore);
    submit_info.setPWaitDstStageMask(&wait_dst_stage_mask);
    submit_info.setCommandBufferCount(1);
    submit_info.setPCommandBuffers(&*mPresentCommandBuffers[result.value]);
    submit_info.setSignalSemaphoreCount(1);
    submit_info.setPSignalSemaphores(&*mRenderingFinishedSemaphore);
    // todo create real presentation queue
    mVulkanGD->mGraphicsQueue.submit({ submit_info }, { });

    vk::PresentInfoKHR present_info;
    present_info.setWaitSemaphoreCount(1);
    present_info.setPWaitSemaphores(&*mRenderingFinishedSemaphore);
    // todo multi window presentation
    present_info.setSwapchainCount(1);
    present_info.setPSwapchains(&*mSwapChain);
    present_info.setPImageIndices(&result.value);
    mVulkanGD->mGraphicsQueue.presentKHR(present_info);
}

uint64_t yuki::VulkanSwapChain::getSurfaceFormat()
{
    return static_cast<uint64_t>(mSurfaceFormat);
}
