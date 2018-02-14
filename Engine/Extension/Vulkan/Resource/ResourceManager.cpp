#include <iostream>
#include <cassert>

#include <Usagi/Engine/Extension/Vulkan/Device/Device.hpp>

#include "ResourceManager.hpp"
#include "StreamBuffer.hpp"

namespace yuki::extension::vulkan
{
vk::UniqueBuffer ResourceManager::createBuffer(const std::size_t size,
    const vk::BufferUsageFlags &usages)
{
    vk::BufferCreateInfo buffer_create_info;

    buffer_create_info.setSize(size);
    buffer_create_info.setUsage(usages);
    buffer_create_info.setSharingMode(vk::SharingMode::eExclusive);

    return mDevice->device().createBufferUnique(buffer_create_info);
}

vk::UniqueDeviceMemory ResourceManager::createHeapMemory(
    const std::size_t size,
    const vk::MemoryPropertyFlags &mem_properties,
    const vk::BufferUsageFlags &usages)
{
    auto device = mDevice->device();
    auto buffer = createBuffer(size, usages);
    auto physical_device = mDevice->physicalDevice();

    const auto buffer_memory_requirements = device.getBufferMemoryRequirements(
        buffer.get());
    const auto memory_properties = physical_device.getMemoryProperties();

    // find first heap 
    for(uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
    {
        // the i-th bit is set only when that memory type is supported.
        // https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkMemoryRequirements.html
        if((buffer_memory_requirements.memoryTypeBits & 1 << i) &&
            (memory_properties.memoryTypes[i].propertyFlags & mem_properties))
        {
            vk::MemoryAllocateInfo memory_allocate_info;
            memory_allocate_info.setAllocationSize(buffer_memory_requirements.size);
            memory_allocate_info.setMemoryTypeIndex(i);

            try
            {
                return device.allocateMemoryUnique(memory_allocate_info);
            }
            catch(const vk::OutOfHostMemoryError &e)
            {
                std::cout << e.what() << std::endl;
                std::cout << "[WARNING] Memory space in the host is used up!" << std::
                    endl;
            }
            catch(const vk::OutOfDeviceMemoryError &e)
            {
                std::cout << e.what() << std::endl;
                std::cout << "[WARNING] Memory space in heap " << memory_properties.
                    memoryTypes[i].heapIndex << " is used up!" << std::endl;
            }
        }
    }
    throw std::bad_alloc();
}

void ResourceManager::createHeapMemories()
{
    mStableMemory = createHeapMemory(ALLOCATION_UNIT_SIZE,
        vk::MemoryPropertyFlagBits::eDeviceLocal, STREAM_USAGES);
    mDynamicMemory = createHeapMemory(ALLOCATION_UNIT_SIZE,
        vk::MemoryPropertyFlagBits::eHostVisible,
        vk::BufferUsageFlagBits::eVertexBuffer |
        vk::BufferUsageFlagBits::eIndexBuffer |
        vk::BufferUsageFlagBits::eUniformBuffer);
    mStagingMemory = createHeapMemory(STAGING_AREA_SIZE,
        vk::MemoryPropertyFlagBits::eHostVisible, STAGING_USAGES);

    vk::BufferCreateInfo buffer_create_info;
    buffer_create_info.setSize(STAGING_AREA_SIZE);
    buffer_create_info.setUsage(STAGING_USAGES);
    buffer_create_info.setSharingMode(vk::SharingMode::eExclusive);

    auto device = mDevice->device();
    mStagingBuffer = device.createBufferUnique(buffer_create_info);
    auto memory_requirements = device.getBufferMemoryRequirements(mStagingBuffer.get());
    // todo: verify the buffer alignment is correct

    device.bindBufferMemory(mStagingBuffer.get(), mStagingMemory.get(), 0);
    mMappedStagingMemory = device.mapMemory(mStagingMemory.get(), 0, STAGING_AREA_SIZE);
}

void ResourceManager::createCommandPool()
{
    vk::CommandPoolCreateInfo pool_create_info;
    pool_create_info.setFlags(
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer |
        vk::CommandPoolCreateFlagBits::eTransient);
    pool_create_info.setQueueFamilyIndex(mDevice->getGraphicsQueueFamilyIndex());
    mCopyCommandPool = mDevice->device().createCommandPoolUnique(pool_create_info);
}

void ResourceManager::createBatches()
{
    vk::CommandBufferAllocateInfo command_buffer_allocate_info;
    command_buffer_allocate_info.setCommandPool(mCopyCommandPool.get());
    command_buffer_allocate_info.setCommandBufferCount(NUM_COMMAND_BATCHES);
    command_buffer_allocate_info.setLevel(vk::CommandBufferLevel::ePrimary);

    auto buffers = mDevice->device().allocateCommandBuffersUnique(
        command_buffer_allocate_info);

    vk::FenceCreateInfo fence_create_info;

    for(std::size_t i = 0; i < buffers.size(); ++i)
    {
        mBatches.emplace_back(std::move(buffers[i]),
            mDevice->device().createFenceUnique(fence_create_info));
    }
}

ResourceManager::ResourceManager(Device *device)
    : mDevice { device }
    , mStableAllocator { nullptr, ALLOCATION_UNIT_SIZE, 64 * 1024 /* 64 KiB */, 64 }
    , mDynamicMemoryAllocator {
        nullptr,
        ALLOCATION_UNIT_SIZE,
        64 * 1024 /* 64 KiB */,
        64
    }
    , mStagingAllocator { nullptr, STAGING_AREA_SIZE }
{
    createHeapMemories();
    createCommandPool();
    createBatches();
}

ResourceManager::~ResourceManager()
{
    mDevice->device().unmapMemory(mStagingMemory.get());
}

std::unique_ptr<graphics::DynamicBuffer> ResourceManager::createDynamicBuffer(
    std::size_t size, DynamicBuffer::UsageFlag usage)
{
    switch(usage)
    {
        case DynamicBuffer::UsageFlag::STREAM:
            return std::make_unique<StreamBuffer>(this, size);
        case DynamicBuffer::UsageFlag::DYNAMIC:
        default:
            throw std::logic_error("not implemented");
    }
}

void * ResourceManager::allocateStagingMemory(const std::size_t size)
{
    // todo handle allocation failure
    auto offset = mStagingAllocator.allocate(size);
    // todo use as base address of mStagingAllocator
    return static_cast<char*>(mMappedStagingMemory)
        + reinterpret_cast<std::size_t>(offset);
}

vk::UniqueBuffer ResourceManager::allocateStreamBuffer(const std::size_t size,
    std::size_t *offset)
{
    assert(offset != nullptr);

    auto buffer = createBuffer(size, STREAM_USAGES);
    auto device = mDevice->device();
    const auto memory_requirements = device.getBufferMemoryRequirements(
        buffer.get());

    const auto allocated_offset = reinterpret_cast<std::size_t>(mStableAllocator.allocate(
        memory_requirements.size,
        memory_requirements.alignment
    ));
    device.bindBufferMemory(buffer.get(), mStableMemory.get(), allocated_offset);

    *offset = allocated_offset;
    return std::move(buffer);
}

void ResourceManager::freeStreamBuffer(std::size_t offset)
{
    mStableAllocator.deallocate(reinterpret_cast<void*>(offset));
}

ResourceManager::CopyCommandBatch::CopyCommandBatch(vk::UniqueCommandBuffer buffer,
    vk::UniqueFence fence)
    : cmd_buffer { std::move(buffer), std::move(fence) }
{
    cmd_buffer.setNotifyPolicy([](auto h, auto p)
    {
        h->onResourceStreamed(p);
    });
}

void ResourceManager::CopyCommandBatch::releaseStagingAreas(
    memory::CircularAllocator &allocator)
{
    for(auto &&s : staging_areas)
    {
        allocator.deallocate(s);
    }
}

void ResourceManager::CopyCommandBatch::reset()
{
    num_batched_commands = 0;
}

void ResourceManager::CopyCommandBatch::cmdCopyBuffer(vk::Buffer src_buffer,
    vk::Buffer dst_buffer, uint32_t region_count, const vk::BufferCopy *regions)
{
    assert(num_batched_commands < NUM_COMMANDS_PER_BATCH);

    cmd_buffer.commandBuffer().copyBuffer(src_buffer, dst_buffer, region_count, regions);
    ++num_batched_commands;
}

// todo swap buffers after uploading is done.
// todo copy other resources. e.g. textures/images.
void ResourceManager::enqueueCopyBuffer(vk::Buffer src_buffer, vk::Buffer dst_buffer,
    uint32_t region_count, const vk::BufferCopy *regions, ResourceEventHandler *handler,
    user_param_t callback_param)
{
    std::lock_guard<std::mutex> lock(mBatchMutex);

    // if the queue is busy, find a command buffer and batch the command
    // otherwise find a idle command buffer and submit immediately
    for(auto i = mBatches.begin(); i != mBatches.end(); ++i)
    {
        if(!i->cmd_buffer.working())
        {
            if(i->cmd_buffer.idle())
            {
                vk::CommandBufferBeginInfo command_buffer_begin_info;
                command_buffer_begin_info.setFlags(
                    vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
                i->cmd_buffer.commandBuffer().begin(command_buffer_begin_info);
            }

            i->cmdCopyBuffer(src_buffer, dst_buffer, region_count, regions);
            if(handler != nullptr)
                i->cmd_buffer.addFinishHandler(handler, callback_param);

            // submit batch
            if(i->full() || mNumActiveBatches == 0)
            {
                i->cmd_buffer.addFinishHandler(this, i - mBatches.begin());
                i->cmd_buffer.commandBuffer().end();
                i->cmd_buffer.submit(mDevice);
                ++mNumActiveBatches; // todo outofsync on exception?
            }
            return;
        }
    }
    // todo this should not happen
    throw std::runtime_error("no available command buffer");
}

void ResourceManager::commitStagedBuffer(void *staging_area, vk::Buffer target_buffer,
    const std::size_t offset, const std::size_t size, ResourceEventHandler *handler,
    user_param_t callback_param)
{
    vk::BufferCopy copy;
    copy.setSrcOffset(
        // todo use get staging allocator base address
        static_cast<char*>(staging_area) - static_cast<char*>(mMappedStagingMemory)
    );
    copy.setDstOffset(offset);
    copy.setSize(size);

    enqueueCopyBuffer(mStagingBuffer.get(), target_buffer, 1, &copy,
        handler, callback_param);
}

void ResourceManager::onResourceStreamed(user_param_t batch_index)
{
    auto &batch = mBatches[batch_index];

    assert(batch.cmd_buffer.working());

    // the batch won't be mofified when is WORKING, no need to lock.
    batch.releaseStagingAreas(mStagingAllocator);

    // prevent wrong decision on submitting the batch to the device
    std::lock_guard<std::mutex> listener_lock(mBatchMutex);

    batch.reset();
    --mNumActiveBatches;
}
}
