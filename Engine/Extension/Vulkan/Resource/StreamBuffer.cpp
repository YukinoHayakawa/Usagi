#include <cassert>

#include <Usagi/Engine/Extension/Vulkan/Device/Device.hpp>

#include "ResourceManager.hpp"
#include "StreamBuffer.hpp"

namespace yuki::extension::vulkan
{
void StreamBuffer::PrimitiveBuffer::init(ResourceManager *resource_manager,
    std::size_t size)
{
    buffer = resource_manager->allocateStreamBuffer(size, &memory_offset);

    vk::SemaphoreCreateInfo semaphore_create_info;

    available_semaphore = resource_manager->device()->device().createSemaphoreUnique(
        semaphore_create_info);
}

bool StreamBuffer::PrimitiveBuffer::safeToModify() const
{
    return state == graphics::ResourceState::UNINITIALIZED
        || state == graphics::ResourceState::IDLE;
}

bool StreamBuffer::PrimitiveBuffer::safeToRead() const
{
    return state == graphics::ResourceState::IDLE
        || state == graphics::ResourceState::WORKING;
}

std::size_t StreamBuffer::backBufferIndex() const
{
    return !mFrontBufferIndex;
}

StreamBuffer::PrimitiveBuffer & StreamBuffer::backBuffer()
{
    return mBuffers[backBufferIndex()];
}

StreamBuffer::PrimitiveBuffer & StreamBuffer::frontBuffer()
{
    return mBuffers[mFrontBufferIndex];
}

void StreamBuffer::swapBuffers()
{
    assert(mStagingArea == nullptr);

    mFrontBufferIndex = !mFrontBufferIndex;

    assert(frontBuffer().safeToRead());
}

StreamBuffer::StreamBuffer(ResourceManager *resource_manager, std::size_t size)
    : DynamicBuffer { resource_manager, size }
{
    for(auto &buffer : mBuffers)
    {
        buffer.init(resource_manager, size);
    }
}

StreamBuffer::~StreamBuffer()
{
    for(auto &buffer : mBuffers)
    {
        mResourceManager->freeStreamBuffer(buffer.memory_offset);
    }
}

void * StreamBuffer::map(std::size_t offset, std::size_t size)
{
    std::lock_guard<std::mutex> lock(mMutex);

    assert(mStagingArea == nullptr);
    assert(offset + size <= mSize);

    // the back buffer may be the swapped out front buffer
    // being used by the device, in which case we does not try
    // to update it.
    // todo this may cause stutter?
    if(!backBuffer().safeToModify())
    {
        return nullptr;
    }

    mStagingArea = mResourceManager->allocateStagingMemory(size);
    mMappedOffset = offset;
    mMappedSize = size;

    return mStagingArea;
}

void StreamBuffer::unmap()
{
    std::lock_guard<std::mutex> lock(mMutex);

    assert(mStagingArea != nullptr);
    assert(backBuffer().safeToModify());

    backBuffer().state = graphics::ResourceState::STREAMING;
    // todo the unchanged content of front buffer not copied to the back buffer. see SwapFlag
    mResourceManager->commitStagedBuffer(
        mStagingArea, backBuffer().buffer.get(), mMappedOffset, mMappedSize,
        this, backBufferIndex()
    );
    mStagingArea = nullptr;
    mMappedOffset = 0;
    mMappedSize = 0;
}

void StreamBuffer::onResourceStreamed(user_param_t buffer_index)
{
    std::lock_guard<std::mutex> lock(mMutex);

    assert(backBufferIndex() == buffer_index);
    assert(backBuffer().state == graphics::ResourceState::STREAMING);

    backBuffer().state = graphics::ResourceState::IDLE;
    swapBuffers();

    // what if the resource is streamed twice when the front buffer is being used?
    // - swap buffer when the rendering is done?
    // - triple buffering?
}

void StreamBuffer::onResourceReleased(user_param_t buffer_index)
{
}

Buffer::BindInfo StreamBuffer::getLatestBindInfo()
{
    std::lock_guard<std::mutex> lock(mMutex);

    assert(frontBuffer().safeToRead());

    return { frontBuffer().buffer.get(), 0, { } };
}
}
