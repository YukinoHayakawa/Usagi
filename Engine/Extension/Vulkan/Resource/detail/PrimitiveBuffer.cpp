#include "PrimitiveBuffer.hpp"

namespace yuki::extension::vulkan::detail
{
PrimitiveBuffer::PrimitiveBuffer(vk::UniqueBuffer buffer, const std::size_t offset)
    : mBuffer { std::move(buffer) }
    , mOffset { offset }
{
}

vk::Buffer PrimitiveBuffer::bufferHandle() const
{
    return mBuffer.get();
}

std::size_t PrimitiveBuffer::offset() const
{
    return mOffset;
}

graphics::ResourceState PrimitiveBuffer::state() const
{
    return mState;
}

void PrimitiveBuffer::setState(graphics::ResourceState state)
{
    mState = state;
}
}
