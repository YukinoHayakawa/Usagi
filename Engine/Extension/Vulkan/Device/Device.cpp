#include <iostream>

#include <Usagi/Engine/Core/Logging.hpp>
#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>
#include <Usagi/Engine/Runtime/Exception.hpp>
#include <Usagi/Engine/Runtime/Graphics/Device/Waitable.hpp>
#include <Usagi/Engine/Utility/Flag.hpp>
#include <Usagi/Engine/Core/Logging.hpp>

#include "SwapChain.hpp"
#include "../Workload/Pipeline.hpp"
#include "../Workload/CommandPool.hpp"
#include "../Workload/CommandList.hpp"
#include "../Resource/FrameController.hpp"
#include "Fence.hpp"

#include "Device.hpp"
#include <Usagi/Engine/Extension/Vulkan/Resource/ResourceManager.hpp>

namespace yuki::extension::vulkan
{
VkBool32 Device::_debugLayerCallback(VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code,
    const char *layerPrefix, const char *msg, void *userData)
{
    const vk::DebugReportFlagsEXT level(static_cast<vk::DebugReportFlagBitsEXT>(flags));
    if(level & vk::DebugReportFlagBitsEXT::eInformation)
    {
        LOG(INFO) << msg;
    }
    if(level & vk::DebugReportFlagBitsEXT::eWarning)
    {
        LOG(WARNING) << msg;
    }
    if(level & vk::DebugReportFlagBitsEXT::ePerformanceWarning)
    {
        LOG(WARNING) << "(PERFORMANCE)" << msg;
    }
    if(level & vk::DebugReportFlagBitsEXT::eError)
    {
        LOG(ERROR) << msg;
    }
    if(level & vk::DebugReportFlagBitsEXT::eDebug)
    {
        LOG(DEBUG) << msg;
    }

    return VK_FALSE;
}

void Device::_createInstance()
{
    vk::ApplicationInfo application_info;
    application_info.setPApplicationName("Usagi Engine World Editor");
    application_info.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
    application_info.setPEngineName("Usagi Engine");
    application_info.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
    application_info.setApiVersion(VK_API_VERSION_1_0);

    std::cout << "Creating Vulkan intance" << std::endl;

    std::cout << "\nAvailable instance extensions:" << std::endl;
    std::cout << "==============================" << std::endl;
    for(auto &&ext : vk::enumerateInstanceExtensionProperties())
    {
        std::cout << "    " << ext.extensionName << std::endl;
    }

    std::cout << "\nAvailable validation layers:" << std::endl;
    std::cout << "============================" << std::endl;
    for(auto &&layer : vk::enumerateInstanceLayerProperties())
    {
        std::cout << "    " << layer.layerName << ": " << layer.description << std::endl;
    }

    vk::InstanceCreateInfo instance_create_info;
    instance_create_info.setPApplicationInfo(&application_info);

    const std::vector<const char *> instance_extensions
    {
        "VK_KHR_surface",
        // application window
        "VK_KHR_win32_surface",
        // todo remove OS dependency
        "VK_EXT_debug_report",
        // provide feedback from validation layer
    };
    instance_create_info.setEnabledExtensionCount(instance_extensions.size());
    instance_create_info.setPpEnabledExtensionNames(instance_extensions.data());

    // todo: enumerate layers first
    // todo: disable in release
    const std::vector<const char*> validationLayers
    {
        "VK_LAYER_LUNARG_standard_validation"
    };
    instance_create_info.setEnabledLayerCount(validationLayers.size());
    instance_create_info.setPpEnabledLayerNames(validationLayers.data());

    mInstance = vk::createInstanceUnique(instance_create_info);
}

void Device::_createDebugReport()
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
    debug_info.setPfnCallback(&_debugLayerCallback);
    mDebugReportCallback = mInstance->createDebugReportCallbackEXTUnique(debug_info);
}

void Device::_selectPhysicalDevice()
{
    auto physical_devices = mInstance->enumeratePhysicalDevices();
    for(auto &&dev : physical_devices)
    {
        auto prop = dev.getProperties();
        std::cout
            << "\n  * Device Name:    " << prop.deviceName
            << "\n    Device Type:    " << to_string(prop.deviceType)
            << "\n    Device ID:      " << prop.deviceID
            << "\n    API Version:    " << prop.apiVersion
            << "\n    Vendor ID:      " << prop.vendorID
            << "\n    Driver Version: " << prop.driverVersion
            << "\n" << std::endl;

        // todo: select device based on features and score them / let the user choose
        if(!mPhysicalDevice)
            mPhysicalDevice = dev;
        else if(prop.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            mPhysicalDevice = dev;
    }
    if(!mPhysicalDevice)
        throw GraphicsAPIUnsupportedFeatureException() << FeatureDescriptionInfo(
            "no suitable Vulkan device was found!");
    std::cout << "Using device: " << mPhysicalDevice.getProperties().deviceName << std::
        endl;
}

void Device::selectQueue(std::vector<vk::QueueFamilyProperties> &queue_family,
    const vk::QueueFlags &queue_flags, std::size_t *selected_index)
{
    for(auto iter = queue_family.begin(); iter != queue_family.end(); ++iter)
    {
        if(utility::matchAllFlags(iter->queueFlags, queue_flags))
        {
            *selected_index = iter - queue_family.begin();
            return;
        }
    }
    throw std::runtime_error("requested queue type not found");
}

void Device::createDeviceAndQueues()
{
    LOG(INFO) << "Creating logical device";

    auto queue_families = mPhysicalDevice.getQueueFamilyProperties();
    LOG(INFO) << "Supported queue families:";
    for(auto &&qf : queue_families)
    {
        LOG(INFO) << qf.queueCount << ": " << to_string(qf.queueFlags);
    }

    std::size_t graphics_queue_index;

    selectQueue(queue_families,
        vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer,
        &graphics_queue_index);
    LOG(INFO) << "Using queue " << graphics_queue_index << " for graphics and transfer";

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
    device_create_info.setEnabledExtensionCount(device_extensions.size());
    device_create_info.setPpEnabledExtensionNames(device_extensions.data());

    mDevice = mPhysicalDevice.createDeviceUnique(device_create_info);
    mGraphicsQueue = mDevice->getQueue(graphics_queue_index, 0);
    mGraphicsQueueFamilyIndex = graphics_queue_index;
}

Device::Device()
{
    _createInstance();
    _createDebugReport();
    _selectPhysicalDevice();
    createDeviceAndQueues();
}

Device::~Device()
{
    Device::waitIdle();
}

std::unique_ptr<graphics::SwapChain> Device::createSwapChain(Window *window)
{
    if(auto native_window = dynamic_cast<Win32Window*>(window))
    {
        return std::make_unique<SwapChain>(this,
            native_window->getProcessInstanceHandle(),
            native_window->getNativeWindowHandle());
    }
    throw OSAPIUnsupportedPlatformException() << PlatformDescriptionInfo(
        "The windowing system on Win32 is not native.");
}

std::unique_ptr<graphics::Pipeline> Device::createPipeline(
    const graphics::PipelineCreateInfo &info)
{
    return Pipeline::create(this, info);
}

std::unique_ptr<graphics::ResourceManager> Device::createResourceManager()
{
    return std::make_unique<ResourceManager>(this);
}

std::unique_ptr<graphics::CommandPool> Device::createGraphicsCommandPool()
{
    return std::make_unique<CommandPool>(this);
}

std::unique_ptr<graphics::FrameController> Device::createFrameController(
    size_t num_frames)
{
    return std::make_unique<FrameController>(this, 2);
}

//
//std::unique_ptr<graphics::Sampler> Device::createSampler(
//    const graphics::SamplerCreateInfo &info)
//{
//    return Sampler::create(this, info);
//}

void Device::submitGraphicsCommandList(
    graphics::CommandList *command_list,
    const std::vector<graphics::Waitable *> &wait_semaphores,
    const std::vector<graphics::Waitable *> &signal_semaphores,
    class graphics::Waitable *fence
)
{
    CommandList *vulkan_cmd_list = dynamic_cast<CommandList*>(command_list);
    if(!vulkan_cmd_list)
        throw MismatchedSubsystemComponentException() <<
            SubsystemInfo("Rendering") << ComponentInfo("VulkanCommandList");

    std::vector<vk::PipelineStageFlags> wait_stages;
    std::vector<vk::Semaphore> vulkan_wait_semaphores = Semaphore::
            _convertToVulkanHandles(wait_semaphores, &wait_stages),
        vulkan_signal_semaphores = Semaphore::_convertToVulkanHandles(signal_semaphores,
            nullptr);

    auto buffer = vulkan_cmd_list->_getCommandBuffer();
    vk::SubmitInfo submit_info;
    submit_info.setCommandBufferCount(1);
    submit_info.setPCommandBuffers(&buffer);
    submit_info.setWaitSemaphoreCount(vulkan_wait_semaphores.size());
    submit_info.setPWaitSemaphores(vulkan_wait_semaphores.data());
    submit_info.setPWaitDstStageMask(wait_stages.data());
    submit_info.setSignalSemaphoreCount(vulkan_signal_semaphores.size());
    submit_info.setPSignalSemaphores(vulkan_signal_semaphores.data());

    mGraphicsQueue.submit({ submit_info }, dynamic_cast<Fence*>(fence)->fence);
}

void Device::waitIdle()
{
    mDevice->waitIdle();
}

uint32_t Device::getGraphicsQueueFamilyIndex() const
{
    return mGraphicsQueueFamilyIndex;
}

vk::Device Device::device() const
{
    return mDevice.get();
}

vk::PhysicalDevice Device::physicalDevice() const
{
    return mPhysicalDevice;
}

vk::Instance Device::_getInstance() const
{
    return mInstance.get();
}

vk::Queue Device::_getGraphicsQueue() const
{
    return mGraphicsQueue;
}

void Device::submitCommandBuffer(vk::CommandBuffer command_buffer, vk::Fence fence)
{
    std::lock_guard<std::mutex> lock(mSubmitMutex);

    vk::SubmitInfo submit_info;
    submit_info.setCommandBufferCount(1);
    submit_info.setPCommandBuffers(&command_buffer);

    mGraphicsQueue.submit(submit_info, fence);
}
}
