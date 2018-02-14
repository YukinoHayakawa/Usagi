#include <Usagi/Engine/Extension/Vulkan/Device/Device.hpp>

#include "Job.hpp"

namespace yuki::extension::vulkan::detail
{
Job::Job(vk::UniqueCommandBuffer buffer,
    vk::UniqueFence fence)
    : mCommandBuffer { std::move(buffer) }
    , mFinishFence { std::move(fence) }
{
}

void Job::joinFinishWatcher()
{
    if(mFinishWatcher.joinable())
        mFinishWatcher.join();
}

Job::~Job()
{
    joinFinishWatcher();
}

void Job::addWaitSemaphore(vk::Semaphore semaphore, vk::PipelineStageFlags stages)
{
    mWaitSemaphores.push_back(semaphore);
    mWaitStages.push_back(std::move(stages));
}

void Job::addSignalSemaphore(vk::Semaphore semaphore)
{
    mSignalSemaphores.push_back(semaphore);
}

void Job::addFinishHandler(
    graphics::ResourceEventHandler *handler,
    graphics::ResourceEventHandler::user_param_t param)
{
    assert(state == State::IDLE);

    mFinishHandlers.push_back({ handler, param });
}

void Job::submit(Device *device)
{
    assert(state == State::IDLE);

    device->device().resetFences({ mFinishFence.get() });

    vk::SubmitInfo submit_info;

    auto cmd_buffer = mCommandBuffer.get();
    submit_info.setCommandBufferCount(1);
    submit_info.setPCommandBuffers(&cmd_buffer);
    submit_info.setWaitSemaphoreCount(mWaitSemaphores.size());
    submit_info.setPWaitSemaphores(mWaitSemaphores.data());
    submit_info.setPWaitDstStageMask(mWaitStages.data());
    submit_info.setSignalSemaphoreCount(mSignalSemaphores.size());
    submit_info.setPSignalSemaphores(mSignalSemaphores.data());

    device->submitToPrimaryQueue(submit_info, mFinishFence.get());
    state = State::WORKING;

    joinFinishWatcher(); // in case the watcher is a bit slower
    mFinishWatcher = std::thread([=]()
    {
        device->device().waitForFences({ mFinishFence.get() }, true, -1);
        onExecutionFinish();
    });
}

void Job::notifyResourceRelease()
{
    if(!mNotifyPolicy) return;

    for(auto &&h : mFinishHandlers)
    {
        mNotifyPolicy(h.handler, h.param);
    }
}

void Job::onExecutionFinish()
{
    notifyResourceRelease();
    reset();
}

void Job::reset()
{
    mWaitStages.clear();
    mWaitSemaphores.clear();
    mSignalSemaphores.clear();
    mFinishHandlers.clear();
    state = State::IDLE;
}
}
