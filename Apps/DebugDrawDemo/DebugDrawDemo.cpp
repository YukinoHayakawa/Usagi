#include "DebugDrawDemo.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Filesystem/FilesystemAssetPackage.hpp>
#include <Usagi/Camera/CameraComponent.hpp>
#include <Usagi/Camera/ModelViewCameraController.hpp>
#include <Usagi/Camera/PerspectiveCamera.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Runtime/Graphics/Enum/GpuBufferFormat.hpp>
#include <Usagi/Runtime/Graphics/Enum/GraphicsPipelineStage.hpp>
#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/RenderPassCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/Framebuffer.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>
#include <Usagi/Runtime/Graphics/GpuImageCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/GraphicsCommandList.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Runtime/Input/InputManager.hpp>
#include <Usagi/Runtime/Input/Keyboard/Keyboard.hpp>
#include <Usagi/Runtime/Input/Mouse/Mouse.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Window/Window.hpp>
#include <Usagi/Runtime/Window/WindowManager.hpp>
#include <Usagi/Transform/TransformComponent.hpp>

#include <Usagi/Extension/DebugDraw/DebugDrawSubsystem.hpp>

#include "DebugDrawDemoComponent.hpp"

void usagi::DebugDrawDemo::createWindow()
{
    runtime()->initWindow();
    mWindow = runtime()->windowManager()->createWindow(
        u8"🐰 - DebugDraw Demo",
        Vector2i { 100, 100 },
        Vector2u32 { 1920, 1080 }
    );
    mWindow->addEventListener(this);
}

void usagi::DebugDrawDemo::setupInput()
{
    runtime()->initInput();
    const auto mouse = runtime()->inputManager()->virtualMouse();
    mouse->addEventListener(&mInputMap);
    mouse->addEventListener(this);
}

void usagi::DebugDrawDemo::setupGraphics()
{
    runtime()->initGpu();
    auto gpu = runtime()->gpu();
    mSwapchain = gpu->createSwapchain(mWindow.get());
    createRenderTargets();
}

void usagi::DebugDrawDemo::createRenderTargets()
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

void usagi::DebugDrawDemo::setupDebugDraw()
{
    assets()->addChild<FilesystemAssetPackage>("dd", "Data/debugdraw");
    mDebugDraw = addSubsystem("dd", std::make_unique<DebugDrawSubsystem>(
        this
    ));
    RenderPassCreateInfo mAttachments;
    mAttachments.attachment_usages.emplace_back(
        mSwapchain->format(),
        1,
        GpuImageLayout::UNDEFINED,
        GpuImageLayout::PRESENT,
        GpuAttachmentLoadOp::CLEAR,
        GpuAttachmentStoreOp::STORE
    ).clear_color = Color4f::Zero();
    mAttachments.attachment_usages.emplace_back(
        GpuBufferFormat::D32_SFLOAT, // todo get from image params
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

void usagi::DebugDrawDemo::setupCamera()
{
    using namespace std::placeholders;

    mCameraElement = rootElement()->addChild("Camera");
    mCameraTransform = mCameraElement->addComponent<TransformComponent>();
    mCamera = std::make_shared<PerspectiveCamera>();
    mCameraElement->addComponent<CameraComponent>(mCamera);
    mCameraController = std::make_unique<ModelViewCameraController>(
        *mCameraTransform, Vector3f::Zero(), 10.f);
    mInputMap.addAnalogAction2D("Camera:Move", std::bind(
        &ModelViewCameraController::rotate,
        mCameraController.get(), _1));
    mInputMap.bindMouseRelativeMovement("Camera:Move");
}

usagi::DebugDrawDemo::DebugDrawDemo(Runtime *runtime)
    : Game { runtime }
{
    createWindow();
    setupInput();
    setupGraphics();
    setupDebugDraw();
    setupCamera();
}

usagi::DebugDrawDemo::~DebugDrawDemo()
{
}

void usagi::DebugDrawDemo::onMouseButtonStateChange(const MouseButtonEvent &e)
{
    if(e.button == MouseButtonCode::RIGHT)
        e.mouse->setImmersiveMode(e.pressed);
}

void usagi::DebugDrawDemo::onWindowResizeEnd(const WindowSizeEvent &e)
{
    if(e.size.x() != 0 && e.size.y() != 0)
        createRenderTargets();
}

void usagi::DebugDrawDemo::run()
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

        const auto size = framebuffer->size().cast<float>();
        const auto aspect = size.x() / size.y();
        mCamera->setMatrix(degreesToRadians(90.f), aspect, 0.5f, 100.f);
        mDebugDraw->setWorldToNDC(
            // world -> camera local -> NDC
            mCamera->projectionMatrix() *
            mCameraTransform->local_to_parent.inverse());
        mDebugDraw->setRenderSizes(mWindow->size(), framebuffer->size());
        update(dt);

        // Record command lists
        std::vector<std::shared_ptr<GraphicsCommandList>> cmd_lists;
        const auto cmd_inserter = [&](auto cmd_list) {
            cmd_lists.push_back(std::move(cmd_list));
        };
        mDebugDraw->render(dt, framebuffer, cmd_inserter);

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
