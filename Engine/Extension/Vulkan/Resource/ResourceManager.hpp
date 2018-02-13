#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/ResourceManager.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/ResourceEventHandler.hpp>
#include <Usagi/Engine/Runtime/Memory/BitmapAllocator.hpp>
#include <Usagi/Engine/Runtime/Memory/CircularAllocator.hpp>

namespace yuki::extension::vulkan
{
class ResourceManager
    : public graphics::ResourceManager
    , public graphics::ResourceEventHandler
{
    // todo decide during runtime
    static constexpr std::size_t ALLOCATION_UNIT_SIZE = 256 * 1024 * 1024; // 256MiB
    static constexpr std::size_t STAGING_AREA_SIZE = ALLOCATION_UNIT_SIZE;

    class Device *mDevice = nullptr;

    // todo: multiple blocks
    // allocating a large buffer for pooling various types of objects
    // https://developer.nvidia.com/vulkan-memory-management
    // https://gpuopen.com/vulkan-device-memory/

    vk::UniqueDeviceMemory mStableMemory;
    memory::BitmapAllocator mStableAllocator;
    vk::UniqueDeviceMemory mDynamicMemory;
    memory::BitmapAllocator mDynamicMemoryAllocator;
    vk::UniqueDeviceMemory mStagingMemory;
    vk::UniqueBuffer mStagingBuffer;
    memory::CircularAllocator mStagingAllocator;
    void *mMappedStagingMemory = nullptr;

    // using auto as type kills msvc(14.12.25827)
    static inline vk::BufferUsageFlags STREAM_USAGES =
        vk::BufferUsageFlagBits::eTransferDst |
        vk::BufferUsageFlagBits::eVertexBuffer |
        vk::BufferUsageFlagBits::eIndexBuffer |
        vk::BufferUsageFlagBits::eUniformBuffer;
    static inline vk::BufferUsageFlags STAGING_USAGES =
        vk::BufferUsageFlagBits::eTransferSrc;

    vk::UniqueCommandPool mCopyCommandPool;
    static constexpr std::size_t NUM_COMMAND_BATCHES = 16;

    class CopyCommandBatch
    {
        static constexpr std::size_t NUM_COMMANDS_PER_BATCH = 16;

        vk::UniqueCommandBuffer command_buffer;
        vk::UniqueFence finish_fence;
        std::thread fence_listener;
        uint16_t num_batched_commands = 0;

        struct EventHandlerRegistry
        {
            ResourceEventHandler *handler;
            user_param_t param;
        };

        std::vector<EventHandlerRegistry> handlers;
        std::vector<void*> staging_areas;

        enum class State : uint8_t
        {
            IDLE,
            WORKING,
        } state = State::IDLE;

        void joinFenceListener();

    public:
        CopyCommandBatch(vk::UniqueCommandBuffer buffer, vk::UniqueFence fence);
        CopyCommandBatch(CopyCommandBatch &&other) = default;
        CopyCommandBatch & operator=(CopyCommandBatch &&other) = default;
        ~CopyCommandBatch();

        bool idle() const;
        bool full() const;
        void beginCommandBuffer();
        void cmdCopyBuffer(vk::Buffer src_buffer, vk::Buffer dst_buffer,
            uint32_t region_count, const vk::BufferCopy *regions);
        void addBatchFinishHandler(ResourceEventHandler *handler, user_param_t param);

        void submit(Device *device);

        void notifyCopyFinish();
        void reset();
        void releaseStagingAreas(memory::CircularAllocator &allocator);
        void onCopyFinish();
    };

    std::vector<CopyCommandBatch> mBatches;
    std::size_t mNumActiveBatches = 0;
    std::mutex mBatchMutex;

    vk::UniqueBuffer createBuffer(std::size_t size,
        const vk::BufferUsageFlags &usages);
    vk::UniqueDeviceMemory createHeapMemory(std::size_t size,
        const vk::MemoryPropertyFlags &mem_properties,
        const vk::BufferUsageFlags &usages);
    void createHeapMemories();
    void createCommandPool();
    void createBatches();

    void enqueueCopyBuffer(vk::Buffer src_buffer, vk::Buffer dst_buffer,
        uint32_t region_count, const vk::BufferCopy *regions,
        ResourceEventHandler *handler, user_param_t callback_param);

public:
    ResourceManager(Device *device);
    ~ResourceManager();

    std::unique_ptr<graphics::DynamicBuffer> createDynamicBuffer(std::size_t size,
        graphics::DynamicBuffer::UsageFlag usage) override;

    void * allocateStagingMemory(std::size_t size);
    vk::UniqueBuffer allocateStreamBuffer(std::size_t size, std::size_t *offset);
    void freeStreamBuffer(std::size_t offset);

    /**
     * \brief Copy from staging area to the target buffer.
     * \param staging_area An address allocated using allocateStagingMemory().
     * \param target_buffer The caller must ensure that this buffer lives longer
     * than the copy operation.
     * \param offset offset + size <= the size of target_buffer.
     * \param size the copying size. must <= the size of staging area.
     * \param handler When the uploading is done, the onResourceStreamed() method
     * of this handler will be called asynchronizedly.
     * \param callback_param The value that will be used to invoke onResourceStreamed()
     * with.
     */
    void commitStagedBuffer(void *staging_area, vk::Buffer target_buffer,
        std::size_t offset, std::size_t size, ResourceEventHandler *handler,
        user_param_t callback_param);

    void onResourceStreamed(user_param_t batch_index) override;
};
}
