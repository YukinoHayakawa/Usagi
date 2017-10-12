#pragma once

#include "VulkanBuffer.hpp"

namespace yuki
{

class VulkanVertexBuffer : public VulkanBuffer, public VertexBuffer
{
public:
    VulkanVertexBuffer(size_t size, vk::Device device, vk::PhysicalDevice physical_device);
};

}
