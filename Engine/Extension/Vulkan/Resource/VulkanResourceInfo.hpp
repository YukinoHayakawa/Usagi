#pragma once

#include <variant>

#include <vulkan/vulkan.hpp>

namespace usagi
{
using VulkanResourceInfo = std::variant<
    vk::DescriptorImageInfo,
    vk::DescriptorBufferInfo
>;
}
