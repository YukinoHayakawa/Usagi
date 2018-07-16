#include <vulkan/vulkan.hpp>

/*
 * VK_EXT_debug_report
 */

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(
    VkInstance instance,
    const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugReportCallbackEXT *pCallback)
{
    if(auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT")))
    {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(
    VkInstance instance,
    VkDebugReportCallbackEXT callback,
    const VkAllocationCallbacks *pAllocator)
{
    if(auto func = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT")))
    {
        func(instance, callback, pAllocator);
    }
}

VKAPI_ATTR void VKAPI_CALL vkDebugReportMessageEXT(
    VkInstance instance,
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t object,
    size_t location,
    int32_t messageCode,
    const char *pLayerPrefix,
    const char *pMessage)
{
    if(auto func = reinterpret_cast<PFN_vkDebugReportMessageEXT>(
        vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT")))
    {
        func(instance, flags, objectType, object, location, messageCode,
            pLayerPrefix, pMessage);
    }
}
