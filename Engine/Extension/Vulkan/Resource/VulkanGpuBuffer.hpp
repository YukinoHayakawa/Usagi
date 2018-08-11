#pragma once

#include <memory>

#include <Usagi/Engine/Runtime/Graphics/Resource/GpuBuffer.hpp>

namespace usagi
{
class VulkanMemoryPool;
class VulkanGpuDevice;
class VulkanBufferAllocation;

class VulkanGpuBuffer : public GpuBuffer
{
    VulkanMemoryPool * mPool = nullptr;
    std::shared_ptr<VulkanBufferAllocation> mAllocation;

public:
    explicit VulkanGpuBuffer(VulkanMemoryPool *pool);

    void allocate(std::size_t size) override;
    void * getMappedMemory() override;
    void flush() override;

    std::shared_ptr<VulkanBufferAllocation> allocation() const
    {
        return mAllocation;
    }
};
}
