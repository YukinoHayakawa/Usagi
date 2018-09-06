#include "DebugDrawDemo.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Package/Filesystem/FilesystemAssetPackage.hpp>
#include <Usagi/Camera/Controller/ModelViewCameraController.hpp>
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

void usagi::DebugDrawDemo::setupInput()
{
    const auto mouse = runtime()->inputManager()->virtualMouse();
    mouse->addEventListener(&mInputMap);
    mouse->addEventListener(this);
}

void usagi::DebugDrawDemo::createRenderTargets()
{
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
    RenderPassCreateInfo attachments;
    attachments.attachment_usages.emplace_back(
        mSwapchain->format(),
        1,
        GpuImageLayout::UNDEFINED,
        GpuImageLayout::PRESENT,
        GpuAttachmentLoadOp::CLEAR,
        GpuAttachmentStoreOp::STORE
    ).clear_color = Color4f::Zero();
    attachments.attachment_usages.emplace_back(
        GpuBufferFormat::D32_SFLOAT, // todo get from image params
        1,
        GpuImageLayout::UNDEFINED,
        GpuImageLayout::DEPTH_STENCIL_ATTACHMENT,
        GpuAttachmentLoadOp::CLEAR,
        GpuAttachmentStoreOp::STORE
    ).clear_color = { 1, 0, 0, 0 };
    mDebugDraw->createPipelines(attachments);

    mDebugDrawRoot = rootElement()->addChild("DebugDrawRoot");
    mDebugDrawRoot->addComponent<DebugDrawDemoComponent>();
}

void usagi::DebugDrawDemo::setupCamera()
{
    using namespace std::placeholders;

    mCameraElement = rootElement()->addChild<ModelCameraMan>(
        "Camera",
        std::make_shared<PerspectiveCamera>(),
        std::make_shared<ModelViewCameraController>(
            Vector3f::Zero(), 10.f
    ));
    mInputMap.addAnalogAction2D("Camera:Move", std::bind(
        &ModelViewCameraController::rotate,
        mCameraElement->cameraController(), _1));
    mInputMap.bindMouseRelativeMovement("Camera:Move");
}

usagi::DebugDrawDemo::DebugDrawDemo(Runtime *runtime)
    : GraphicalGame {
        runtime,
        u8"🐰 - DebugDraw Demo",
        Vector2i { 100, 100 },
        Vector2u32 { 1920, 1080 }
    }
{
    setupInput();
    createRenderTargets();
    setupDebugDraw();
    setupCamera();
}

usagi::DebugDrawDemo::~DebugDrawDemo()
{
    const auto mouse = runtime()->inputManager()->virtualMouse();
    mouse->removeEventListener(&mInputMap);
    mouse->removeEventListener(this);
}

void usagi::DebugDrawDemo::onMouseButtonStateChange(const MouseButtonEvent &e)
{
    if(e.button == MouseButtonCode::RIGHT)
        e.mouse->setImmersiveMode(e.pressed);
}

void usagi::DebugDrawDemo::run()
{
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
        mCameraElement->camera()->setPerspective(
            degreesToRadians(90.f), aspect, 0.5f, 100.f);
        mDebugDraw->setWorldToNDC(
            // world -> camera local -> NDC
            mCameraElement->camera()->localToNDC() *
            mCameraElement->comp<TransformComponent>()->worldToLocal());
        mDebugDraw->setRenderSizes(mWindow->size(), framebuffer->size());
        update(mMasterClock);

        // Record command lists
        std::vector<std::shared_ptr<GraphicsCommandList>> cmd_lists;
        const auto cmd_inserter = [&](auto cmd_list) {
            cmd_lists.push_back(std::move(cmd_list));
        };
        mDebugDraw->render(mMasterClock, framebuffer, cmd_inserter);

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
        mMasterClock.tick();
    }
}
