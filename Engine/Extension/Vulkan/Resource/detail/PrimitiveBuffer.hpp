#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/ResourceState.hpp>

namespace yuki::extension::vulkan::detail
{
class PrimitiveBuffer
{
    vk::UniqueBuffer mBuffer;
    std::size_t mOffset = 0;
    graphics::ResourceState mState = graphics::ResourceState::UNINITIALIZED;

public:
    PrimitiveBuffer() = default;
    PrimitiveBuffer(vk::UniqueBuffer buffer, std::size_t offset);

    vk::Buffer bufferHandle() const;
    std::size_t offset() const;
    graphics::ResourceState state() const;
    void setState(graphics::ResourceState state);
};
}
