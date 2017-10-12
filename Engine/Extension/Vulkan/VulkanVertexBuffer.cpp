#include <Usagi/Engine/Runtime/Exception.hpp>

#include "VulkanVertexBuffer.hpp"

yuki::VulkanVertexBuffer::VulkanVertexBuffer(size_t size, vk::Device device, vk::PhysicalDevice physical_device) try
    : VulkanBuffer { vk::BufferUsageFlagBits::eVertexBuffer, size, device, physical_device }
{
}
catch(boost::exception &e)
{
    e << MemoryUsageInfo("Vertex Buffer");
    throw;
}
