#pragma once

#include <thread>

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/ResourceEventHandler.hpp>

namespace yuki::extension::vulkan
{
class Device;
}

namespace yuki::extension::vulkan::detail
{
/**
 * \brief Help synchornize resources used in a command buffer.
 * Monitor the progress of command execution and signal the handlers
 * when the execution is done.
 * This class done not handle the recording of the command buffer.
 */
class Job
{
    vk::UniqueCommandBuffer mCommandBuffer;

    std::vector<vk::Semaphore> mWaitSemaphores;
    std::vector<vk::PipelineStageFlags> mWaitStages;
    std::vector<vk::Semaphore> mSignalSemaphores;
    vk::UniqueFence mFinishFence;
    std::thread mFinishWatcher;

    struct EventHandlerRegistry
    {
        graphics::ResourceEventHandler *handler;
        graphics::ResourceEventHandler::user_param_t param;
    };

    using NotifyPolicyFunction = std::function<void(
        graphics::ResourceEventHandler *handler,
        graphics::ResourceEventHandler::user_param_t param)>;
    NotifyPolicyFunction mNotifyPolicy;
    std::vector<EventHandlerRegistry> mFinishHandlers;

    enum class State : uint8_t
    {
        IDLE,
        WORKING,
    } state = State::IDLE;

    void joinFinishWatcher();
    void onExecutionFinish();
    void notifyResourceRelease();
    void reset();

public:
    Job(vk::UniqueCommandBuffer buffer, vk::UniqueFence fence);
    Job(Job &&other) = default;
    Job & operator=(Job &&other) = default;
    ~Job();

    void setNotifyPolicy(const NotifyPolicyFunction &notify_policy)
    {
        mNotifyPolicy = notify_policy;
    }

    bool working() const { return state == State::WORKING; }
    bool idle() const { return state == State::IDLE; }

    vk::CommandBuffer commandBuffer() { return mCommandBuffer.get(); }

    void addWaitSemaphore(vk::Semaphore semaphore, vk::PipelineStageFlags stages);
    void addSignalSemaphore(vk::Semaphore semaphore);
    void addFinishHandler(graphics::ResourceEventHandler *handler,
        graphics::ResourceEventHandler::user_param_t param);

    /**
     * \brief Reset the finish fence and fire the finish watcher thread.
     * The command buffer should be submitted externally.
     * \param device 
     */
    void submit(Device *device);
};
}
