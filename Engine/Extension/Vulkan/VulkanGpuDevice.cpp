#include "VulkanGpuDevice.hpp"

#include <algorithm>

#include <Usagi/Engine/Core/Logging.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImageView.hpp>
#include <Usagi/Engine/Utility/Flag.hpp>
#include <Usagi/Engine/Utility/TypeCast.hpp>

#include "Resource/VulkanFramebuffer.hpp"
#include "Resource/VulkanGpuBuffer.hpp"
#include "Resource/VulkanGpuCommandPool.hpp"
#include "Resource/VulkanGpuImageView.hpp"
#include "Resource/VulkanGraphicsCommandList.hpp"
#include "Resource/VulkanMemoryPool.hpp"
#include "Resource/VulkanSemaphore.hpp"
#include "VulkanEnumTranslation.hpp"
#include "VulkanGraphicsPipelineCompiler.hpp"
#include "VulkanHelper.hpp"
#include "VulkanRenderPass.hpp"

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

VkBool32 usagi::VulkanGpuDevice::debugLayerCallbackDispatcher(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT obj_type,
    uint64_t obj, size_t location, int32_t code,
    const char *layer_prefix, const char *msg, void *user_data)
{
    const auto device = reinterpret_cast<VulkanGpuDevice*>(user_data);
    return device->debugLayerCallback(
        static_cast<vk::DebugReportFlagBitsEXT>(flags),
        static_cast<vk::DebugReportObjectTypeEXT>(obj_type),
        obj,
        location,
        code,
        layer_prefix,
        msg
    );
}

bool usagi::VulkanGpuDevice::debugLayerCallback(
    vk::DebugReportFlagsEXT flags,
    vk::DebugReportObjectTypeEXT obj_type,
    std::uint64_t obj,
    std::size_t location,
    std::int32_t code,
    const char *layer_prefix,
    const char *msg) const
{
    if(flags & vk::DebugReportFlagBitsEXT::eInformation)
    {
        // LOG(info, "Vulkan: {}", msg);
    }
    if(flags & vk::DebugReportFlagBitsEXT::eWarning)
    {
        LOG(warn, "Vulkan: {}", msg);
    }
    if(flags & vk::DebugReportFlagBitsEXT::ePerformanceWarning)
    {
        LOG(warn, "Vulkan: (Perf) {}", msg);
    }
    if(flags & vk::DebugReportFlagBitsEXT::eError)
    {
        LOG(error, "Vulkan: {}", msg);
    }
    if(flags & vk::DebugReportFlagBitsEXT::eDebug)
    {
        LOG(debug, "Vulkan: {}", msg);
    }
    return false;
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

    const std::vector<const char *> instance_extensions
    {
        // application window
        "VK_KHR_surface",
        // todo remove OS dependency, move to VulkanWin32WSI
        "VK_KHR_win32_surface",
        // provide feedback from validation layer
        "VK_EXT_debug_report",
    };
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
    vk::DebugReportCallbackCreateInfoEXT debug_info;
    debug_info.setFlags(
        vk::DebugReportFlagBitsEXT::eInformation |
        vk::DebugReportFlagBitsEXT::eWarning |
        vk::DebugReportFlagBitsEXT::ePerformanceWarning |
        vk::DebugReportFlagBitsEXT::eError |
        vk::DebugReportFlagBitsEXT::eDebug |
        vk::DebugReportFlagBitsEXT(0)
    );
    debug_info.setPfnCallback(&debugLayerCallbackDispatcher);
    debug_info.pUserData = this;
    mDebugReportCallback =
        mInstance->createDebugReportCallbackEXTUnique(debug_info);
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
        "VK_KHR_swapchain",
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
    mDynamicMemoryPool = std::make_unique<VulkanMemoryPool>(
        this,
        1024 * 1024 * 512, // 512MiB  todo from config
        vk::MemoryPropertyFlagBits::eHostVisible,
        vk::BufferUsageFlagBits::eVertexBuffer |
        vk::BufferUsageFlagBits::eIndexBuffer |
        vk::BufferUsageFlagBits::eUniformBuffer
    );
}

usagi::VulkanGpuDevice::VulkanGpuDevice()
{
    createInstance();
    createDebugReport();
    selectPhysicalDevice();
    createDeviceAndQueues();
    createMemoryPools();
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
    const RenderPassCreateInfo &info,
    const Vector2u32 &size,
    std::initializer_list<GpuImageView *> views)
{
    auto render_pass {
        std::make_shared<VulkanRenderPass>(this, info)
    };

    const auto vk_views = vulkan::transformObjects(views,
        [&](auto v) {
            return dynamic_cast_ref<VulkanGpuImageView>(v).view();
        }
    );
    vk::FramebufferCreateInfo fb_info;
    fb_info.setRenderPass(render_pass->renderPass());
    fb_info.setAttachmentCount(static_cast<uint32_t>(vk_views.size()));
    fb_info.setPAttachments(vk_views.data());
    fb_info.setWidth(size.x());
    fb_info.setHeight(size.y());
    fb_info.setLayers(1);

    return std::make_shared<VulkanFramebuffer>(
        std::move(render_pass),
        mDevice->createFramebufferUnique(fb_info),
        size);
}

// todo sem pool
std::shared_ptr<usagi::GpuSemaphore> usagi::VulkanGpuDevice::createSemaphore()
{
    auto sem = mDevice->createSemaphoreUnique(vk::SemaphoreCreateInfo { });
    return std::make_unique<VulkanSemaphore>(std::move(sem));
}

std::shared_ptr<usagi::GpuBuffer> usagi::VulkanGpuDevice::createBuffer()
{
    return std::make_shared<VulkanGpuBuffer>(mDynamicMemoryPool.get());
}

void usagi::VulkanGpuDevice::submitGraphicsJobs(
    std::vector<std::shared_ptr<GraphicsCommandList>> jobs,
    std::initializer_list<std::shared_ptr<GpuSemaphore>> wait_semaphores,
    std::initializer_list<GraphicsPipelineStage> wait_stages,
    std::initializer_list<std::shared_ptr<GpuSemaphore>> signal_semaphores)
{
    const auto vk_jobs = vulkan::transformObjects(jobs, [&](auto &&j) {
        return dynamic_cast_ref<VulkanGraphicsCommandList>(j).commandBuffer();
    });
    const auto vk_wait_sems = vulkan::transformObjects(wait_semaphores,
        [&](auto &&s) {
            return dynamic_cast_ref<VulkanSemaphore>(s).semaphore();
        }
    );
    const auto vk_wait_stages = vulkan::transformObjects(wait_stages,
        [&](auto &&s) {
            // todo wait on multiple stages
            return vk::PipelineStageFlags(translate(s));
        }
    );
    const auto vk_signal_sems = vulkan::transformObjects(signal_semaphores,
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
