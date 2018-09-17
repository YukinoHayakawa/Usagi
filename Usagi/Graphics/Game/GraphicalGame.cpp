#include "GraphicalGame.hpp"

#include <algorithm>

#include <Usagi/Game/GameStateManager.hpp>
#include <Usagi/Graphics/RenderTarget/RenderTargetDescriptor.hpp>
#include <Usagi/Graphics/RenderTarget/Source/ImageRenderTargetSource.hpp>
#include <Usagi/Graphics/RenderTarget/Source/SwapchainRenderTargetSource.hpp>
#include <Usagi/Runtime/Graphics/Enum/GraphicsPipelineStage.hpp>
#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Window/Window.hpp>

void usagi::GraphicalGame::createMainWindow(
    const std::string &window_title,
    const Vector2i &window_position,
    const Vector2u32 &window_size,
    const GpuBufferFormat swapchain_format)
{
    assert(!mMainWindow.window);

    mMainWindow.create(
        mRuntime.get(),
        window_title,
        window_position,
        window_size,
        swapchain_format
    );
    mMainWindow.window->addEventListener(this);
}

void usagi::GraphicalGame::setupRenderTargets(const bool depth)
{
    mSize = mMainWindow.swapchain->size();

    // add default color output target
    addSharedSource("color", std::make_shared<SwapchainRenderTargetSource>(
        mMainWindow.swapchain.get()));
    // add depth target
    if(depth)
    {
        GpuImageCreateInfo info;
        info.size = mMainWindow.window->size();
        info.format = GpuBufferFormat::D32_SFLOAT;
        info.usage = GpuImageUsage::DEPTH_STENCIL_ATTACHMENT;
        addSharedSource("depth", std::make_shared<ImageRenderTargetSource>(
            mRuntime->gpu(), info));
    }

    // clear images & perform transitions
    {
        RenderTargetDescriptor desc { this };
        {
            GpuAttachmentUsage u;
            u.initial_layout = GpuImageLayout::UNDEFINED;
            u.final_layout = GpuImageLayout::COLOR_ATTACHMENT;
            u.op.load_op = GpuAttachmentLoadOp::CLEAR;
            desc.sharedColorTarget("swapchain", u);
        }
        if(depth)
        {
            GpuAttachmentUsage u;
            u.initial_layout = GpuImageLayout::UNDEFINED;
            u.final_layout = GpuImageLayout::DEPTH_STENCIL_ATTACHMENT;
            u.op.load_op = GpuAttachmentLoadOp::CLEAR;
            u.op.clear_color = { 1, 0, 0, 0 };
            desc.depthTarget(u);
        }
        mPreRender->createRenderTarget(desc);
    }
    {
        RenderTargetDescriptor desc { this };
        {
            GpuAttachmentUsage u;
            u.initial_layout = GpuImageLayout::COLOR_ATTACHMENT;
            u.final_layout = GpuImageLayout::PRESENT;
            desc.sharedColorTarget("swapchain", u);
        }
        mPostRender->createRenderTarget(desc);
    }
}

bool usagi::GraphicalGame::continueGame() const
{
    return mMainWindow.window->isOpen();
}

usagi::GraphicalGame::GraphicalGame(std::shared_ptr<Runtime> runtime)
    : Game(std::move(runtime))
{
    mRuntime->initGpu();

    mPreRender = std::make_unique<ImageTransitionSubsystem>(mRuntime->gpu());
    mPostRender = std::make_unique<ImageTransitionSubsystem>(mRuntime->gpu());
}

usagi::GraphicalGame::~GraphicalGame()
{
    if(mMainWindow.window)
        mMainWindow.window->removeEventListener(this);
}

void usagi::GraphicalGame::submitGraphicsJobs(
    std::vector<std::shared_ptr<GraphicsCommandList>> &jobs)
{
    std::move(jobs.begin(), jobs.end(), std::back_inserter(mPendingJobs));
    jobs.clear();
}

void usagi::GraphicalGame::frame()
{
    processInput();

    // switch swapchain image
    const auto wait_semaphores = { mMainWindow.swapchain->acquireNextImage() };

    // update states & gather render jobs
    mPendingJobs.push_back(mPreRender->render(mMasterClock));
    mStateManager->update(mMasterClock);
    mPendingJobs.push_back(mPostRender->render(mMasterClock));
    // remove empty lists
    mPendingJobs.erase(std::remove(
        mPendingJobs.begin(), mPendingJobs.end(), nullptr), mPendingJobs.end());

    // submit graphics jobs
    const auto wait_stages = {
        GraphicsPipelineStage::COLOR_ATTACHMENT_OUTPUT
    };
    const auto gpu_device = mRuntime->gpu();
    const auto rendering_finished_sem = gpu_device->createSemaphore();
    const auto signal_semaphores = {
        rendering_finished_sem
    };
    gpu_device->submitGraphicsJobs(
        mPendingJobs,
        wait_semaphores,
        wait_stages,
        signal_semaphores
    );
    mPendingJobs.clear();

    // Present image
    mMainWindow.swapchain->present({ rendering_finished_sem });

    // collect unused resources from previous frames
    gpu_device->reclaimResources();

    updateClock();
}

usagi::GpuDevice * usagi::GraphicalGame::gpu() const
{
    return mRuntime->gpu();
}

void usagi::GraphicalGame::onWindowResizeEnd(const WindowSizeEvent &e)
{
    if(e.size.x() != 0 && e.size.y() != 0)
        resize(e.size);
}
