#include "VulkanGpuDevice.hpp"

#include <algorithm>

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Runtime/Graphics/GpuImageView.hpp>
#include <Usagi/Runtime/Graphics/GpuSamplerCreateInfo.hpp>
#include <Usagi/Runtime/Memory/BitmapMemoryAllocator.hpp>
#include <Usagi/Utility/Flag.hpp>
#include <Usagi/Utility/TypeCast.hpp>
#include <Usagi/Utility/String.hpp>

#include "VulkanFramebuffer.hpp"
#include "VulkanGpuBuffer.hpp"
#include "VulkanGpuCommandPool.hpp"
#include "VulkanGpuImageView.hpp"
#include "VulkanGraphicsCommandList.hpp"
#include "VulkanMemoryPool.hpp"
#include "VulkanPooledImage.hpp"
#include "VulkanSampler.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanEnumTranslation.hpp"
#include "VulkanGraphicsPipelineCompiler.hpp"
#include "VulkanHelper.hpp"
#include "VulkanRenderPass.hpp"

using namespace usagi::vulkan;

VkBool32 usagi::VulkanGpuDevice::debugMessengerCallbackDispatcher(
    const VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    const VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *user_data)
{
    const auto device = reinterpret_cast<VulkanGpuDevice*>(user_data);
    return device->debugMessengerCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT { message_severity },
        vk::DebugUtilsMessageTypeFlagsEXT  { message_type },
        reinterpret_cast<const vk::DebugUtilsMessengerCallbackDataEXT *>(
            callback_data));
}

VkBool32 usagi::VulkanGpuDevice::debugMessengerCallback(
    const vk::DebugUtilsMessageSeverityFlagsEXT &message_severity,
    const vk::DebugUtilsMessageTypeFlagsEXT &message_type,
    const vk::DebugUtilsMessengerCallbackDataEXT *callback_data) const
{
    using namespace logging;

    // see https://www.lunarg.com/wp-content/uploads/2018/05/Vulkan-Debug-Utils_05_18_v1.pdf for example code

    LoggingLevel level;
    using Severity = vk::DebugUtilsMessageSeverityFlagBitsEXT;
    using Type = vk::DebugUtilsMessageTypeFlagBitsEXT;
    bool validation_error = false;

    if(message_severity & Severity::eVerbose)
        level = LoggingLevel::debug;
    else if(message_severity & Severity::eInfo)
        level = LoggingLevel::info;
    else if(message_severity & Severity::eWarning)
        level = LoggingLevel::warn;
    else if(message_severity & Severity::eError)
        level = LoggingLevel::error;
    else
        level = LoggingLevel::info;

    log(level,
        "[Vulkan] {} : {} - Message ID Number {}, Message ID String {}:\n{}",
        to_string(message_severity),
        to_string(message_type),
        callback_data->messageIdNumber,
        USAGI_OPT_STRING(callback_data->pMessageIdName),
        callback_data->pMessage);

    if(callback_data->objectCount > 0)
    {
        log(level, "    Objects - {}",
            callback_data->objectCount);
        for(uint32_t i = 0; i < callback_data->objectCount; ++i)
        {
            const auto &object = callback_data->pObjects[i];
            log(level, "        Object[{}] - Type {}, Value {}, Name \"{}\"",
                i,
                to_string(object.objectType),
                object.objectHandle,
                USAGI_OPT_STRING(object.pObjectName)
            );
        }
    }
    if(callback_data->cmdBufLabelCount > 0)
    {
        log(level, "    Command Buffer Label - {}",
            callback_data->cmdBufLabelCount);
        for(uint32_t i = 0; i < callback_data->cmdBufLabelCount; ++i)
        {
            log(level,
                "        Label[{}] - {} {{ {}, {}, {}, {} }}\n",
                i,
                callback_data->pCmdBufLabels[i].pLabelName,
                callback_data->pCmdBufLabels[i].color[0],
                callback_data->pCmdBufLabels[i].color[1],
                callback_data->pCmdBufLabels[i].color[2],
                callback_data->pCmdBufLabels[i].color[3]);
        }
    }

    if(message_type & Type::eValidation) {
        validation_error = true;
    }

    // Don't bail out, but keep going. return false;
    return false;
}

uint32_t usagi::VulkanGpuDevice::selectQueue(
    std::vector<vk::QueueFamilyProperties> &queue_family,
    const vk::QueueFlags &queue_flags)
{
    for(auto iter = queue_family.begin(); iter != queue_family.end(); ++iter)
    {
        if(utility::matchAllFlags(iter->queueFlags, queue_flags))
        {
            return static_cast<uint32_t>(iter - queue_family.begin());
        }
    }
    throw std::runtime_error(
        "Could not find a queue family with required flags.");
}

void usagi::VulkanGpuDevice::createInstance()
{
    LOG(info, "Creating Vulkan intance");
    LOG(info, "--------------------------------");

    vk::ApplicationInfo application_info;
    application_info.setPApplicationName("UsagiEngine");
    application_info.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
    application_info.setPEngineName("Usagi");
    application_info.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
    application_info.setApiVersion(VK_API_VERSION_1_0);

    // Extensions
    {
        LOG(info, "Available instance extensions");
        LOG(info, "--------------------------------");
        for(auto &&ext : vk::enumerateInstanceExtensionProperties())
        {
            LOG(info, ext.extensionName);
        }
        LOG(info, "--------------------------------");
    }
    // Validation layers
    {
        LOG(info, "Available validation layers");
        LOG(info, "--------------------------------");
        for(auto &&layer : vk::enumerateInstanceLayerProperties())
        {
            LOG(info, "Name       : {}", layer.layerName);
            LOG(info, "Description: {}", layer.description);
            LOG(info, "--------------------------------");
        }
    }

    vk::InstanceCreateInfo instance_create_info;
    instance_create_info.setPApplicationInfo(&application_info);

    std::vector<const char *> instance_extensions
    {
        // application window
        VK_KHR_SURFACE_EXTENSION_NAME,
        // provide feedback from validation layer, etc.
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    };
    addPlatformSurfaceExtension(instance_extensions);
    instance_create_info.setEnabledExtensionCount(
        static_cast<uint32_t>(instance_extensions.size()));
    instance_create_info.setPpEnabledExtensionNames(instance_extensions.data());

    // todo: enumerate layers first
    // todo: disable in release
    const std::vector<const char*> validation_layers
    {
        "VK_LAYER_LUNARG_standard_validation"
    };
    instance_create_info.setEnabledLayerCount(
        static_cast<uint32_t>(validation_layers.size()));
    instance_create_info.setPpEnabledLayerNames(validation_layers.data());

    mInstance = createInstanceUnique(instance_create_info);
}

void usagi::VulkanGpuDevice::createDebugReport()
{
    vk::DebugUtilsMessengerCreateInfoEXT info;
    using Severity = vk::DebugUtilsMessageSeverityFlagBitsEXT;
    info.messageSeverity =
        // Severity::eVerbose |
        Severity::eInfo |
        Severity::eWarning |
        Severity::eError;
    using Type = vk::DebugUtilsMessageTypeFlagBitsEXT;
    info.messageType =
        Type::eGeneral |
        Type::eValidation |
        Type::ePerformance;
    info.pfnUserCallback = &debugMessengerCallbackDispatcher;

    mDebugUtilsMessenger = mInstance->createDebugUtilsMessengerEXTUnique(info);
}

void usagi::VulkanGpuDevice::selectPhysicalDevice()
{
    {
        LOG(info, "Available physical devices");
        LOG(info, "--------------------------------");
        auto physical_devices = mInstance->enumeratePhysicalDevices();
        for(auto &&dev : physical_devices)
        {
            const auto prop = dev.getProperties();
            LOG(info, "Device Name   : {}", prop.deviceName);
            LOG(info, "Device Type   : {}", to_string(prop.deviceType));
            LOG(info, "Device ID     : {}", prop.deviceID);
            LOG(info, "API Version   : {}", prop.apiVersion);
            LOG(info, "Driver Version: {}", prop.vendorID);
            LOG(info, "Vendor ID     : {}", prop.vendorID);
            LOG(info, "--------------------------------");
            // todo: select device based on features and score them / let the
            // user choose
            if(!mPhysicalDevice)
                mPhysicalDevice = dev;
            else if(prop.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
                mPhysicalDevice = dev;
        }
    }
    if(!mPhysicalDevice)
        throw std::runtime_error("No available GPU supporting Vulkan.");
    LOG(info, "Using physical device: {}",
        mPhysicalDevice.getProperties().deviceName);
}

void usagi::VulkanGpuDevice::createDeviceAndQueues()
{
    LOG(info, "Creating device and queues");

    auto queue_families = mPhysicalDevice.getQueueFamilyProperties();
    LOG(info, "Supported queue families:");
    for(std::size_t i = 0; i < queue_families.size(); ++i)
    {
        auto &qf = queue_families[i];
        LOG(info, "#{}: {} * {}", i, to_string(qf.queueFlags), qf.queueCount);
    }

    const auto graphics_queue_index = selectQueue(queue_families,
        vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer);
    checkQueuePresentationCapacity(graphics_queue_index);

    LOG(info, "Getting a queue from queue family {}.",
        graphics_queue_index);

    vk::DeviceCreateInfo device_create_info;

    vk::PhysicalDeviceFeatures features;
    device_create_info.setPEnabledFeatures(&features);
    features.setFillModeNonSolid(true);
    features.setLargePoints(true);
    features.setWideLines(true);

    vk::DeviceQueueCreateInfo queue_create_info[1];
    float queue_priority = 1;
    queue_create_info[0].setQueueFamilyIndex(graphics_queue_index);
    queue_create_info[0].setQueueCount(1);
    queue_create_info[0].setPQueuePriorities(&queue_priority);
    device_create_info.setQueueCreateInfoCount(1);
    device_create_info.setPQueueCreateInfos(queue_create_info);

    // todo: check device capacity
    const std::vector<const char *> device_extensions
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    device_create_info.setEnabledExtensionCount(static_cast<uint32_t>(
        device_extensions.size()));
    device_create_info.setPpEnabledExtensionNames(device_extensions.data());

    mDevice = mPhysicalDevice.createDeviceUnique(device_create_info);

    mGraphicsQueue = mDevice->getQueue(graphics_queue_index, 0);
    mGraphicsQueueFamilyIndex = graphics_queue_index;
}

void usagi::VulkanGpuDevice::createMemoryPools()
{
    const auto dyn_size = 1024 * 1024 * 64; // 64MiB  todo from config
    const auto device_size = 1024 * 1024 * 512; // 512MiB  todo from config
    LOG(info, "Allocating {} bytes for dynamic memory pool", dyn_size);
    mDynamicBufferPool = std::make_unique<BitmapBufferPool>(
        this,
        dyn_size,
        vk::MemoryPropertyFlagBits::eHostVisible |
        vk::MemoryPropertyFlagBits::eHostCoherent,
        vk::BufferUsageFlagBits::eTransferSrc |
        vk::BufferUsageFlagBits::eVertexBuffer |
        vk::BufferUsageFlagBits::eIndexBuffer |
        vk::BufferUsageFlagBits::eUniformBuffer,
        [](const vk::MemoryRequirements &req) {
            return std::make_unique<BitmapMemoryAllocator>(
                nullptr,
                req.size,
                32 * 1024 /* 32 KiB */ // todo config
            );
        }
    );

    LOG(info, "Allocating {} bytes for device memory pool", device_size);
    mDeviceImagePool = std::make_unique<BitmapImagePool>(
        this,
        device_size,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eSampled,
        [](const vk::MemoryRequirements &req) {
            return std::make_unique<BitmapMemoryAllocator>(
                nullptr,
                req.size,
                32 * 1024 /* 32 KiB */ // todo config
            );
        }
    );

    {
        vk::CommandPoolCreateInfo info;
        // todo use DMA queue
        info.setQueueFamilyIndex(mGraphicsQueueFamilyIndex);
        info.setFlags(vk::CommandPoolCreateFlagBits::eTransient);
        mTransferCommandPool = mDevice->createCommandPoolUnique(info);
    }
}

void usagi::VulkanGpuDevice::createFallbackTexture()
{
    GpuImageCreateInfo info;
    info.format = GpuBufferFormat::R8G8B8A8_UNORM;
    info.size = { 16, 16 };
    info.usage = GpuImageUsage::SAMPLED;
    mFallbackTexture = createImage(info);
}

usagi::VulkanGpuDevice::VulkanGpuDevice()
{
    createInstance();
    createDebugReport();
    selectPhysicalDevice();
    createDeviceAndQueues();
    createMemoryPools();
    createFallbackTexture();
}

usagi::VulkanGpuDevice::~VulkanGpuDevice()
{
    // Wait till all operations are completed so it is safe to release the
    // resources.
    mDevice->waitIdle();
}

std::unique_ptr<usagi::GraphicsPipelineCompiler> usagi::VulkanGpuDevice::
    createPipelineCompiler()
{
    return std::make_unique<VulkanGraphicsPipelineCompiler>(this);
}

std::shared_ptr<usagi::GpuCommandPool>
    usagi::VulkanGpuDevice::createCommandPool()
{
    return std::make_shared<VulkanGpuCommandPool>(this);
}

std::shared_ptr<usagi::RenderPass> usagi::VulkanGpuDevice::createRenderPass(
    const RenderPassCreateInfo &info)
{
    return std::make_shared<VulkanRenderPass>(this, info);
}

std::shared_ptr<usagi::Framebuffer> usagi::VulkanGpuDevice::createFramebuffer(
    const Vector2u32 &size,
    std::vector<std::shared_ptr<GpuImageView>> views)
{
    auto vk_views = transformObjects(views,
        [&](auto &&v) {
            return dynamic_pointer_cast_throw<VulkanGpuImageView>(v);
        }
    );

    return std::make_shared<VulkanFramebuffer>(this, size, std::move(vk_views));
}

// todo sem pool
std::shared_ptr<usagi::GpuSemaphore> usagi::VulkanGpuDevice::createSemaphore()
{
    auto sem = mDevice->createSemaphoreUnique(vk::SemaphoreCreateInfo { });
    return std::make_unique<VulkanSemaphore>(std::move(sem));
}

std::shared_ptr<usagi::GpuBuffer> usagi::VulkanGpuDevice::createBuffer(
    GpuBufferUsage usage)
{
    return std::make_shared<VulkanGpuBuffer>(mDynamicBufferPool.get(), usage);
}

std::shared_ptr<usagi::GpuImage> usagi::VulkanGpuDevice::createImage(
    const GpuImageCreateInfo &info)
{
    // todo check format availability
    return mDeviceImagePool->createPooledImage(info);
}

std::shared_ptr<usagi::GpuSampler> usagi::VulkanGpuDevice::createSampler(
    const GpuSamplerCreateInfo &info)
{
    vk::SamplerCreateInfo vk_info;
    vk_info.setMagFilter(translate(info.mag_filter));
    vk_info.setMinFilter(translate(info.min_filter));
    vk_info.setMipmapMode(vk::SamplerMipmapMode::eLinear);
    vk_info.setAddressModeU(translate(info.addressing_mode_u));
    vk_info.setAddressModeV(translate(info.addressing_mode_v));
    // todo sampler setBorderColor
    // vk_info.setBorderColor({ });
    return std::make_shared<VulkanSampler>(
        mDevice->createSamplerUnique(vk_info));
}

std::shared_ptr<usagi::GpuImage>
usagi::VulkanGpuDevice::fallbackTexture() const
{
    return mFallbackTexture;
}

void usagi::VulkanGpuDevice::submitGraphicsJobs(
    const std::vector<std::shared_ptr<GraphicsCommandList>> &jobs,
    std::initializer_list<std::shared_ptr<GpuSemaphore>> wait_semaphores,
    std::initializer_list<GraphicsPipelineStage> wait_stages,
    std::initializer_list<std::shared_ptr<GpuSemaphore>> signal_semaphores)
{
    const auto vk_jobs = transformObjects(jobs, [&](auto &&j) {
        return dynamic_cast_ref<VulkanGraphicsCommandList>(j).commandBuffer();
    });
    const auto vk_wait_sems = transformObjects(wait_semaphores,
        [&](auto &&s) {
            return dynamic_cast_ref<VulkanSemaphore>(s).semaphore();
        }
    );
    const auto vk_wait_stages = transformObjects(wait_stages,
        [&](auto &&s) {
            // todo wait on multiple stages
            return vk::PipelineStageFlags(translate(s));
        }
    );
    const auto vk_signal_sems = transformObjects(signal_semaphores,
        [&](auto &&s) {
            return dynamic_cast_ref<VulkanSemaphore>(s).semaphore();
        }
    );

    BatchResourceList batch_resources;
    batch_resources.fence = mDevice->createFenceUnique(vk::FenceCreateInfo { });

    vk::SubmitInfo info;
    info.setCommandBufferCount(static_cast<uint32_t>(vk_jobs.size()));
    info.setPCommandBuffers(vk_jobs.data());
    info.setWaitSemaphoreCount(static_cast<uint32_t>(vk_wait_sems.size()));
    info.setPWaitSemaphores(vk_wait_sems.data());
    info.setSignalSemaphoreCount(static_cast<uint32_t>(vk_signal_sems.size()));
    info.setPSignalSemaphores(vk_signal_sems.data());
    info.setPWaitDstStageMask(vk_wait_stages.data());

    const auto cast_append = [&](auto &&container) {
        std::transform(
            container.begin(), container.end(),
            std::back_inserter(batch_resources.resources),
            [&](auto &&j) {
                return dynamic_pointer_cast_throw<VulkanBatchResource>(j);
            }
        );
    };
    cast_append(jobs);
    cast_append(wait_semaphores);
    cast_append(signal_semaphores);

    mGraphicsQueue.submit({ info }, batch_resources.fence.get());

    mBatchResourceLists.push_back(std::move(batch_resources));
}

void usagi::VulkanGpuDevice::reclaimResources()
{
    for(auto i = mBatchResourceLists.begin(); i != mBatchResourceLists.end();)
    {
        if(mDevice->getFenceStatus(i->fence.get()) == vk::Result::eSuccess)
            i = mBatchResourceLists.erase(i);
        else
            ++i;
    }
}

void usagi::VulkanGpuDevice::waitIdle()
{
    mDevice->waitIdle();
}

uint32_t usagi::VulkanGpuDevice::graphicsQueueFamily() const
{
    return mGraphicsQueueFamilyIndex;
}

vk::Queue usagi::VulkanGpuDevice::presentQueue() const
{
    return mGraphicsQueue;
}

vk::Device usagi::VulkanGpuDevice::device() const
{
    return mDevice.get();
}

vk::PhysicalDevice usagi::VulkanGpuDevice::physicalDevice() const
{
    return mPhysicalDevice;
}

std::shared_ptr<usagi::VulkanBufferAllocation>
usagi::VulkanGpuDevice::allocateStageBuffer(std::size_t size)
{
    return mDynamicBufferPool->allocate(size);
}

void usagi::VulkanGpuDevice::copyBufferToImage(
    const std::shared_ptr<VulkanBufferAllocation> &buffer,
    VulkanGpuImage *image,
    const Vector2i &offset,
    const Vector2u32 &size)
{
    vk::UniqueCommandBuffer cmd;
    {
        vk::CommandBufferAllocateInfo info;
        info.setCommandBufferCount(1);
        info.setCommandPool(mTransferCommandPool.get());
        info.setLevel(vk::CommandBufferLevel::ePrimary);

        cmd = std::move(mDevice->allocateCommandBuffersUnique(info).front());
    }
    {
        vk::CommandBufferBeginInfo info;
        info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmd->begin(info);
    }
    vk::ImageSubresourceRange range;
    range.setAspectMask(vk::ImageAspectFlagBits::eColor);
    range.setBaseArrayLayer(0);
    range.setLayerCount(1);
    range.setBaseMipLevel(0);
    range.setLevelCount(1);
    {
        vk::ImageMemoryBarrier barrier;
        barrier.setImage(image->image());
        barrier.setOldLayout(vk::ImageLayout::eUndefined);
        barrier.setNewLayout(vk::ImageLayout::eTransferDstOptimal);
        barrier.setSrcQueueFamilyIndex(mGraphicsQueueFamilyIndex);
        barrier.setDstQueueFamilyIndex(mGraphicsQueueFamilyIndex);
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setSubresourceRange(range);
        cmd->pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            { }, { }, { }, { barrier });
    }
    {
        vk::BufferImageCopy copy;
        copy.setImageExtent({ size.x(), size.y(), 1 });
        copy.setImageOffset({ offset.x(), offset.y(), 0 });
        copy.setBufferOffset(buffer->offset());
        copy.imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
        copy.imageSubresource.setLayerCount(1);
        copy.imageSubresource.setBaseArrayLayer(0);
        copy.imageSubresource.setMipLevel(0);
        cmd->copyBufferToImage(
            buffer->pool()->buffer(), image->image(),
            vk::ImageLayout::eTransferDstOptimal, { copy });
    }
    {
        vk::ImageMemoryBarrier barrier;
        barrier.setImage(image->image());
        barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
        barrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
        barrier.setSrcQueueFamilyIndex(mGraphicsQueueFamilyIndex);
        barrier.setDstQueueFamilyIndex(mGraphicsQueueFamilyIndex);
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
        barrier.setSubresourceRange(range);
        cmd->pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader,
            { }, { }, { }, { barrier });
    }
    cmd->end();
    {
        vk::SubmitInfo info;
        const auto cmd_handle = cmd.get();
        info.setCommandBufferCount(1);
        info.setPCommandBuffers(&cmd_handle);
        mGraphicsQueue.submit({ info }, { });
    }
    // todo batch uploads & manage dependencies
    mDevice->waitIdle();
}
