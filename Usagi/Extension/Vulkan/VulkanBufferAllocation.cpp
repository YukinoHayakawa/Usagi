#include "VulkanBufferAllocation.hpp"

#include "VulkanMemoryPool.hpp"

usagi::VulkanBufferAllocation::VulkanBufferAllocation(
    VulkanBufferMemoryPoolBase *pool,
    std::size_t offset,
    std::size_t size,
    void *mapped_address)
    : mPool(pool)
    , mOffset(offset)
    , mSize(size)
    , mMappedAddress(mapped_address)
{
}

usagi::VulkanBufferAllocation::~VulkanBufferAllocation()
{
    mPool->deallocate(mOffset);
}
