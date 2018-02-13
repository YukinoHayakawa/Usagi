#pragma once

#include <Usagi/Engine/Runtime/Graphics/Resource/ResourceEventHandler.hpp>

#include "DynamicBuffer.hpp"

namespace yuki::extension::vulkan
{
class StreamBuffer : public DynamicBuffer, public graphics::ResourceEventHandler
{
    // protect accesses from updating thread and notifying thread
    std::mutex mMutex;

    // todo: triple buffering if this hits the performance
    static constexpr auto NUM_BUFFERS = 2;
    detail::PrimitiveBuffer mBuffers[NUM_BUFFERS];
    std::size_t mFrontBufferIndex = 0;

    void *mStagingArea = nullptr;
    std::size_t mMappedOffset = 0, mMappedSize = 0;

    std::size_t backBufferIndex() const;
    detail::PrimitiveBuffer & backBuffer();
    detail::PrimitiveBuffer & frontBuffer();
    void swapBuffers();

public:
    StreamBuffer(ResourceManager *resource_manager, std::size_t size);
    ~StreamBuffer();

    void * map(std::size_t offset, std::size_t size) override;
    void unmap() override;

    void onResourceStreamed(user_param_t user_param) override;

    std::pair<vk::Buffer, std::size_t> getBindInfo() override;
};
}
