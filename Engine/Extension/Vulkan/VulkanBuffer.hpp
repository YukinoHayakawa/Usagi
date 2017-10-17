#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/GraphicsBuffer.hpp>
#include "VulkanMemoryAllocator.hpp"

namespace yuki
{

class VulkanBuffer : public GraphicsBuffer
{
    class GraphicsMemoryAllocator *mAllocator = nullptr;
    GraphicsMemoryAllocator::Allocation mAllocation;

public:
    VulkanBuffer(size_t size, class GraphicsMemoryAllocator *allocator);

    void * getMappedAddress() override;
    void flush() override;
    void reallocate() override;

    vk::Buffer _getBuffer() const;
    size_t _getOffset() const;
};

}
