#pragma once

#include <memory>

#include <Usagi/Runtime/Graphics/GpuBuffer.hpp>
#include <Usagi/Runtime/Graphics/Enum/GpuBufferUsage.hpp>

#include "VulkanShaderResource.hpp"

namespace usagi
{
class VulkanBufferMemoryPoolBase;
class VulkanGpuDevice;
class VulkanBufferAllocation;

// note that VulkanBufferAllocation is inherited from VulkanBatchResource,
// but not this class, which is only a wrapper of the real resource.
class VulkanGpuBuffer
    : public GpuBuffer
    , public VulkanShaderResource
{
    VulkanBufferMemoryPoolBase * mPool = nullptr;
    GpuBufferUsage mUsage;
    std::shared_ptr<VulkanBufferAllocation> mAllocation;

public:
    VulkanGpuBuffer(VulkanBufferMemoryPoolBase *pool, GpuBufferUsage usage);

    void allocate(std::size_t size) override;
    void release() override;
    std::size_t size() const override;

    void * mappedMemory() override;
    void flush() override;

    // only used by uniform buffers
    void fillShaderResourceInfo(
        vk::WriteDescriptorSet &write,
        VulkanResourceInfo &info) override;

    std::shared_ptr<VulkanBufferAllocation> allocation() const
    {
        return mAllocation;
    }
};
}
