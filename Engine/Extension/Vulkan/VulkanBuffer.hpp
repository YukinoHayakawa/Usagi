#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/VertexBuffer.hpp>

namespace yuki
{

class VulkanBuffer : public virtual GraphicsBuffer
{
protected:
    vk::Device mDevice;
    vk::UniqueBuffer mBuffer;
    vk::UniqueDeviceMemory mMemory;
    bool mIsMapped = false;
    size_t mMappingOffset = 0;

public:
    VulkanBuffer::VulkanBuffer(vk::BufferUsageFlags usage, size_t size, vk::Device device, vk::PhysicalDevice physical_device);

    void * map(size_t offset, size_t size) override;
    void unmap() override;

    vk::Buffer _getBuffer() const;
};

}
