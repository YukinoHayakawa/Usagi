#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Graphics/GpuDevice.hpp>

namespace usagi
{
class VulkanGpuDevice : GpuDevice
{
public:
    vk::Device device();
};
}
