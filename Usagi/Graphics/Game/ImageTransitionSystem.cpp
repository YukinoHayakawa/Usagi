#include "ImageTransitionSystem.hpp"

#include <Usagi/Graphics/RenderTarget/RenderTarget.hpp>
#include <Usagi/Graphics/RenderTarget/RenderTargetDescriptor.hpp>
#include <Usagi/Runtime/Graphics/GpuCommandPool.hpp>
#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/GraphicsCommandList.hpp>

void usagi::ImageTransitionSystem::update(const Clock &clock)
{
}

void usagi::ImageTransitionSystem::createRenderTarget(
    RenderTargetDescriptor &descriptor)
{
    mRenderTarget = descriptor.finish();
}

usagi::ImageTransitionSystem::ImageTransitionSystem(GpuDevice *gpu)
{
    mCommandPool = gpu->createCommandPool();
}

void usagi::ImageTransitionSystem::onElementComponentChanged(
    Element *element)
{
}

void usagi::ImageTransitionSystem::createPipelines()
{
}

std::shared_ptr<usagi::GraphicsCommandList>
usagi::ImageTransitionSystem::render(const Clock &clock)
{
    if(!mRenderTarget) return { };

    auto cmd = mCommandPool->allocateGraphicsCommandList();
    cmd->beginRecording();
    cmd->beginRendering(
        mRenderTarget->renderPass(),
        mRenderTarget->createFramebuffer());
    cmd->endRendering();
    cmd->endRecording();
    return std::move(cmd);
}
