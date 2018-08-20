#include <Usagi/Engine/Asset/AssetRoot.hpp>
#include <Usagi/Engine/Asset/Filesystem/FilesystemAssetPackage.hpp>
#include <Usagi/Engine/Core/Logging.hpp>
#include <Usagi/Engine/Game/Game.hpp>
#include <Usagi/Engine/Runtime/Graphics/Enum/GpuBufferFormat.hpp>
#include <Usagi/Engine/Runtime/Graphics/Enum/GraphicsPipelineStage.hpp>
#include <Usagi/Engine/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Engine/Runtime/Graphics/RenderPassCreateInfo.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImage.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImageCreateInfo.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GraphicsCommandList.hpp>
#include <Usagi/Engine/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Engine/Runtime/Input/InputManager.hpp>
#include <Usagi/Engine/Runtime/Input/Keyboard/Keyboard.hpp>
#include <Usagi/Engine/Runtime/Input/Mouse/Mouse.hpp>
#include <Usagi/Engine/Runtime/Runtime.hpp>
#include <Usagi/Engine/Runtime/Window/Window.hpp>
#include <Usagi/Engine/Runtime/Window/WindowManager.hpp>

#include <Usagi/Engine/Extension/DebugDraw/DebugDrawSubsystem.hpp>

#include "DebugDrawDemoComponent.hpp"

using namespace usagi;

class DebugDrawDemo
    : public Game
    , public WindowEventListener
{
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Swapchain> mSwapchain;
    std::shared_ptr<GpuImage> mDepthBuffer;
    DebugDrawSubsystem *mDebugDraw = nullptr;
    RenderPassCreateInfo mAttachments;
    Element *mDebugDrawRoot = nullptr;

    void createRenderTargets()
    {
        mSwapchain->create(mWindow->size(), GpuBufferFormat::R8G8B8A8_UNORM);

        // Create depth buffer
        {
            GpuImageCreateInfo info;
            info.size = mWindow->size();
            info.format = GpuBufferFormat::D32_SFLOAT;
            info.usage = GpuImageUsage::DEPTH_STENCIL_ATTACHMENT;
            mDepthBuffer = runtime()->gpu()->createImage(info);
        }
    }

public:
    explicit DebugDrawDemo(Runtime *runtime)
        : Game { runtime }
    {
        // Setting up window

        runtime->initWindow();
        mWindow = runtime->windowManager()->createWindow(
            u8"🐰 - DebugDraw Demo",
            Vector2i { 100, 100 },
            Vector2u32 { 1920, 1080 }
        );
        mWindow->addEventListener(this);

        // Setting up input

        runtime->initInput();

        // Setting up graphics

        runtime->initGpu();
        auto gpu = runtime->gpu();
        mSwapchain = gpu->createSwapchain(mWindow.get());
        createRenderTargets();

        // Setting up ImGui

        assets()->addChild<FilesystemAssetPackage>("dd", "Data/debugdraw");
        const auto input_manager = runtime->inputManager();
        mDebugDraw = addSubsystem("dd", std::make_unique<DebugDrawSubsystem>(
            this
        ));
        mAttachments.attachment_usages.emplace_back(
            mSwapchain->format(),
            1,
            GpuImageLayout::UNDEFINED,
            GpuImageLayout::PRESENT,
            GpuAttachmentLoadOp::CLEAR,
            GpuAttachmentStoreOp::STORE
        ).clear_color = Color4f::Zero();
        mAttachments.attachment_usages.emplace_back(
            GpuBufferFormat::D32_SFLOAT, // todo from image
            1,
            GpuImageLayout::UNDEFINED,
            GpuImageLayout::DEPTH_STENCIL_ATTACHMENT,
            GpuAttachmentLoadOp::CLEAR,
            GpuAttachmentStoreOp::STORE
        ).clear_color = { 1, 0, 0, 0 };
        mDebugDraw->createPipelines(mAttachments);

        mDebugDrawRoot = rootElement()->addChild("DebugDrawRoot");
        mDebugDrawRoot->addComponent<DebugDrawDemoComponent>();
    }

    void onWindowResizeEnd(const WindowSizeEvent &e) override
    {
        if(e.size.x() != 0 && e.size.y() != 0)
            createRenderTargets();
    }

    void run()
    {
        using Clock = std::chrono::high_resolution_clock;
        if constexpr(!Clock::is_steady)
            LOG(warn, "std::chrono::high_resolution_clock is not steady!");

        auto start = Clock::now();
        TimeDuration dt { 0 };
        while(mWindow->isOpen())
        {
            auto gpu = runtime()->gpu();

            // Process window/input events
            runtime()->windowManager()->processEvents();
            runtime()->inputManager()->processEvents();

            // Create framebuffer
            const auto wait_semaphores = { mSwapchain->acquireNextImage() };
            const auto framebuffer = gpu->createFramebuffer(
                mSwapchain->size(),
                {
                    mSwapchain->currentImage()->baseView(),
                    mDepthBuffer->baseView()
                }
            );

            // Record command lists
            std::vector<std::shared_ptr<GraphicsCommandList>> cmd_lists;
            const auto cmd_inserter = [&](auto cmd_list) {
                cmd_lists.push_back(std::move(cmd_list));
            };
            mDebugDraw->update(dt, framebuffer, cmd_inserter);

            // Submit jobs
            const auto wait_stages = {
                GraphicsPipelineStage::COLOR_ATTACHMENT_OUTPUT
            };
            const auto rendering_finished_sem = gpu->createSemaphore();
            const auto signal_semaphores = {
                rendering_finished_sem
            };
            gpu->submitGraphicsJobs(
                cmd_lists,
                wait_semaphores,
                wait_stages,
                signal_semaphores
            );

            // Present image
            mSwapchain->present({ rendering_finished_sem });

            // GC
            gpu->reclaimResources();

            // Calculate elapsed time
            const auto end = Clock::now();
            dt = std::chrono::duration_cast<TimeDuration>(end - start);
            start = end;
        }
    }
};

int main(int argc, char *argv[])
{
    auto runtime = Runtime::create();
    runtime->enableCrashHandler("DebugDrawDemoErrorDump");
    DebugDrawDemo demo(runtime.get());
    demo.run();
}
