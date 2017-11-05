#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/Buffer.hpp>
#include "MemoryAllocator.hpp"

namespace yuki::vulkan
{

class Buffer : public graphics::Buffer
{
    class graphics::MemoryAllocator *mAllocator = nullptr;
    graphics::MemoryAllocator::Allocation mAllocation;

public:
    Buffer(size_t size, class graphics::MemoryAllocator *allocator);

    void * getMappedAddress() override;
    void flush() override;
    void reallocate() override;

    vk::Buffer _getBuffer() const;
    size_t _getOffset() const;
};

}
