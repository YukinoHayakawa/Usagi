#include "ImageTransitionSubsystem.hpp"

#include <Usagi/Graphics/RenderTarget/RenderTarget.hpp>
#include <Usagi/Graphics/RenderTarget/RenderTargetDescriptor.hpp>
#include <Usagi/Runtime/Graphics/GpuCommandPool.hpp>
#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/GraphicsCommandList.hpp>

void usagi::ImageTransitionSubsystem::update(const Clock &clock)
{
}

void usagi::ImageTransitionSubsystem::createRenderTarget(
    RenderTargetDescriptor &descriptor)
{
    mRenderTarget = descriptor.finish();
}

usagi::ImageTransitionSubsystem::ImageTransitionSubsystem(GpuDevice *gpu)
{
    mCommandPool = gpu->createCommandPool();
}

void usagi::ImageTransitionSubsystem::onElementComponentChanged(
    Element *element)
{
}

void usagi::ImageTransitionSubsystem::createPipelines()
{
}

std::shared_ptr<usagi::GraphicsCommandList>
usagi::ImageTransitionSubsystem::render(const Clock &clock)
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
