#pragma once

#include <Usagi/Engine/Graphics/GpuBuffer.hpp>

namespace usagi
{
class VulkanGpuBuffer : public GpuBuffer
{
    vk::UniqueBuffer mBuffer;

public:
    vk::Buffer buffer() const { return mBuffer.get(); }
};
}
