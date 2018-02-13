#include <cassert>

#include "ResourceManager.hpp"
#include "StreamBuffer.hpp"

namespace yuki::extension::vulkan
{
std::size_t StreamBuffer::backBufferIndex() const
{
    return !mFrontBufferIndex;
}

detail::PrimitiveBuffer & StreamBuffer::backBuffer()
{
    return mBuffers[backBufferIndex()];
}

detail::PrimitiveBuffer & StreamBuffer::frontBuffer()
{
    return mBuffers[mFrontBufferIndex];
}

void StreamBuffer::swapBuffers()
{
    mFrontBufferIndex = !mFrontBufferIndex;
}

StreamBuffer::StreamBuffer(ResourceManager *resource_manager, std::size_t size)
    : DynamicBuffer { resource_manager, size }
{
    std::size_t offset;
    for(auto &buffer : mBuffers)
    {
        auto allocation = mResourceManager->allocateStreamBuffer(size, &offset);
        buffer = detail::PrimitiveBuffer { std::move(allocation), offset };
    }
}

StreamBuffer::~StreamBuffer()
{
    for(auto &buffer : mBuffers)
    {
        mResourceManager->freeStreamBuffer(buffer.offset());
    }
}

void * StreamBuffer::map(std::size_t offset, std::size_t size)
{
    std::lock_guard<std::mutex> lock(mMutex);

    assert(mStagingArea == nullptr);
    assert(offset + size <= mSize);
    assert(backBuffer().state() == graphics::ResourceState::IDLE ||
        backBuffer().state() == graphics::ResourceState::UNINITIALIZED);

    mStagingArea = mResourceManager->allocateStagingMemory(size);
    mMappedOffset = offset;
    mMappedSize = size;

    return mStagingArea;
}

void StreamBuffer::unmap()
{
    std::lock_guard<std::mutex> lock(mMutex);

    assert(mStagingArea != nullptr);
    assert(backBuffer().state() == graphics::ResourceState::IDLE ||
        backBuffer().state() == graphics::ResourceState::UNINITIALIZED);

    backBuffer().setState(graphics::ResourceState::STREAMING);
    // todo the unchanged content of front buffer not copied to the back buffer. see SwapFlag
    mResourceManager->commitStagedBuffer(
        mStagingArea, backBuffer().bufferHandle(), mMappedOffset, mMappedSize,
        this, backBufferIndex()
    );
    mStagingArea = nullptr;
    mMappedOffset = 0;
    mMappedSize = 0;
}

void StreamBuffer::onResourceStreamed(user_param_t user_param)
{
    std::lock_guard<std::mutex> lock(mMutex);

    assert(mStagingArea == nullptr); // not mapped
    assert(user_param == backBufferIndex()); // not swapped
    assert(backBuffer().state() == graphics::ResourceState::STREAMING);

    backBuffer().setState(graphics::ResourceState::IDLE);

    swapBuffers();

    // what if the resource is streamed twice when the front buffer is being used?
    // - swap buffer when the rendering is done?
    // - triple buffering?
}

std::pair<vk::Buffer, std::size_t> StreamBuffer::getBindInfo()
{
    std::lock_guard<std::mutex> lock(mMutex);

    return { frontBuffer().bufferHandle(), 0 };
}
}
