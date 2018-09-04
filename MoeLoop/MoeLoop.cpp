#include "MoeLoop.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Package/Filesystem/FilesystemAssetPackage.hpp>
#include <Usagi/Camera/Controller/StaticCameraController.hpp>
#include <Usagi/Camera/OrthogonalCamera.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Runtime/Graphics/Enum/GpuImageUsage.hpp>
#include <Usagi/Runtime/Graphics/Enum/GraphicsPipelineStage.hpp>
#include <Usagi/Runtime/Graphics/Framebuffer.hpp>
#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>
#include <Usagi/Runtime/Graphics/GpuImageCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/RenderPassCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Runtime/Input/InputManager.hpp>
#include <Usagi/Runtime/Input/Mouse/Mouse.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Window/Window.hpp>
#include <Usagi/Runtime/Window/WindowManager.hpp>
#include <Usagi/Transform/TransformComponent.hpp>

#include "Scene/Scene.hpp"
#include "Scene/Character.hpp"
#include "Scene/ImageLayer.hpp"
#include "Render/SortedSpriteRenderingSubsystem.hpp"
#include "JSON/JsonPropertySheetAssetConverter.hpp"

namespace usagi::moeloop
{
void MoeLoop::setupCamera()
{
    using namespace std::placeholders;

    mCameraElement = rootElement()->addChild<ModelCameraMan>(
        "CameraMan",
        std::make_shared<OrthogonalCamera>(),
        std::make_shared<StaticCameraController>());
    /*mInputMap.addAnalogAction2D("Camera:Move", std::bind(
        &ModelViewCameraController::rotate,
        mCameraElement->cameraController(), _1));
    mInputMap.bindMouseRelativeMovement("Camera:Move");*/
}

void MoeLoop::setupGraphics()
{
    mSpriteRender = addSubsystem(
        "sprite",
        std::make_unique<SortedSpriteRenderingSubsystem>(
            this, [](TransformComponent *lt, SpriteComponent *ls,
                TransformComponent *rt, SpriteComponent *rs) {
                return lt->position().y() > rt->position().y();
            }
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
    mSpriteRender->createPipeline(attachments);

    createRenderTargets();
}

void MoeLoop::createRenderTargets()
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

MoeLoop::MoeLoop(Runtime *runtime)
    : SingleWindowGame(runtime, u8"MoeLoop", { 100, 100 }, { 1920, 1080 })
{
    mSceneRoot = rootElement()->addChild("SceneRoot");
    assets()->addChild<FilesystemAssetPackage>("moeloop", "Data/moeloop");
    auto mouse = runtime->inputManager()->virtualMouse();
    mouse->addEventListener(this);
    mouse->addEventListener(&mInputMap);

    setupCamera();
    setupGraphics();
}

MoeLoop::~MoeLoop()
{
    auto mouse = runtime()->inputManager()->virtualMouse();
    mouse->removeEventListener(this);
    mouse->removeEventListener(&mInputMap);
}

void MoeLoop::mainLoop()
{
    bindScript();

    mCurrentScript = mLuaContext.newThread();
    auto scene_function = mLuaContext.loadfile("init.lua");

    using Clock = std::chrono::high_resolution_clock;
    if constexpr(!Clock::is_steady)
        LOG(warn, "std::chrono::high_resolution_clock is not steady!");

    auto start = Clock::now();
    TimeDuration dt { 0 };
    mCurrentScript(scene_function);
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
        mCameraElement->camera()->setOrthogonal(
            0, size.x(), 0, size.y(), -100, 100);
        mSpriteRender->setWorldToNDC(
            // world -> camera local -> NDC
            mCameraElement->camera()->localToNDC() *
            mCameraElement->transform()->localToWorld().inverse());
        update(dt);

        // Record command lists
        std::vector<std::shared_ptr<GraphicsCommandList>> cmd_lists;
        const auto cmd_inserter = [&](auto cmd_list) {
            cmd_lists.push_back(std::move(cmd_list));
        };
        mSpriteRender->render(dt, framebuffer, cmd_inserter);

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

void MoeLoop::onWindowResizeEnd(const WindowSizeEvent &e)
{
    SingleWindowGame::onWindowResizeEnd(e);
    createRenderTargets();
}

void MoeLoop::onMouseButtonStateChange(const MouseButtonEvent &e)
{
    if(e.button == MouseButtonCode::LEFT && e.pressed)
        continueScript();
}

void MoeLoop::onMouseWheelScroll(const MouseWheelEvent &e)
{
    if(e.distance.y() < 0)
        continueScript();
}

void MoeLoop::bindScript()
{
    using namespace std::placeholders;

    mLuaContext.setErrorHandler([](const int error, const char *msg) {
        LOG(error, "Lua error {}: {}", error, msg);
        throw std::runtime_error("Script error");
    });

    mLuaContext["unimplemented"].setFunction(&MoeLoop::unimplemented);

    mLuaContext["MoeLoop"].setClass(kaguya::UserdataMetatable<MoeLoop>()
        .addFunction("loadScene", &MoeLoop::loadScene)
        // todo not useful
        .addFunction("setCurrentScene", &MoeLoop::setCurrentScene)
        .addFunction("addFilesystemPackage", &MoeLoop::addFilesystemPackage)
    );
    mLuaContext["ml"] = this;

    Scene::exportScript(mLuaContext);
    Character::exportScript(mLuaContext);
    ImageLayer::exportScript(mLuaContext);
}

void MoeLoop::addFilesystemPackage(std::string name, const std::string &path)
{
    assets()->addChild<FilesystemAssetPackage>(
        std::move(name), std::filesystem::u8path(path));
}

Scene * MoeLoop::loadScene(const std::string &name)
{
    LOG(info, "Loading scene: {}", name);
    const auto data = assets()->uncachedRes<JsonPropertySheetAssetConverter>(
        fmt::format("scenes/{}.json", name)
    );
    auto scene = rootElement()->addChild<Scene>(name, runtime(), assets());
    scene->load(data);
    return scene;
}

void MoeLoop::setCurrentScene(Scene *scene)
{
    assert(mSceneRoot->hasChild(scene));

    mCurrentScene = scene;
}

void MoeLoop::unimplemented(const std::string &msg)
{
    LOG(warn, "Script: unimplemented code: {}", msg);
}

void MoeLoop::continueScript()
{
    mCurrentScript.resume<std::string>();
}
}
