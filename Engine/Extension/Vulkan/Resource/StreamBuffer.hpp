#pragma once

#include <Usagi/Engine/Runtime/Graphics/Resource/ResourceEventHandler.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/ResourceState.hpp>

#include "DynamicBuffer.hpp"

namespace yuki::extension::vulkan
{
class StreamBuffer : public DynamicBuffer, public graphics::ResourceEventHandler
{
    struct PrimitiveBuffer
    {
        vk::UniqueBuffer buffer;
        vk::UniqueSemaphore available_semaphore;
        std::size_t memory_offset = 0;
        graphics::ResourceState state = graphics::ResourceState::UNINITIALIZED;

        void init(ResourceManager *resource_manager, std::size_t size);
        bool safeToModify() const;
        bool safeToRead() const;
    };

    // protect accesses from updating thread and notifying thread
    std::mutex mMutex;

    static constexpr auto NUM_BUFFERS = 2;
    PrimitiveBuffer mBuffers[NUM_BUFFERS];
    std::size_t mFrontBufferIndex = 0;

    void *mStagingArea = nullptr;
    std::size_t mMappedOffset = 0, mMappedSize = 0;

    std::size_t backBufferIndex() const;
    PrimitiveBuffer & backBuffer();
    PrimitiveBuffer & frontBuffer();
    void swapBuffers();

public:
    StreamBuffer(ResourceManager *resource_manager, std::size_t size);
    ~StreamBuffer();

    void * map(std::size_t offset, std::size_t size) override;
    void unmap() override;

    void onResourceStreamed(user_param_t buffer_index) override;
    void onResourceReleased(user_param_t buffer_index) override;

    BindInfo getLatestBindInfo() override;
};
}
