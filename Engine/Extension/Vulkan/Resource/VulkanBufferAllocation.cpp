#include "VulkanBufferAllocation.hpp"

#include "VulkanMemoryPool.hpp"

usagi::VulkanBufferAllocation::VulkanBufferAllocation(
    VulkanBufferMemoryPoolBase *pool,
    std::size_t offset,
    std::size_t size,
    void *apped_address)
    : mPool(pool)
    , mOffset(offset)
    , mSize(size)
    , mMappedAddress(apped_address)
{
}

usagi::VulkanBufferAllocation::~VulkanBufferAllocation()
{
    mPool->deallocate(mOffset);
}
