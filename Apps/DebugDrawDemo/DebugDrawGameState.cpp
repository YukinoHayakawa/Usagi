#include "DebugDrawGameState.hpp"

#include <Usagi/Camera/Controller/ModelViewCameraController.hpp>
#include <Usagi/Camera/PerspectiveCamera.hpp>
#include <Usagi/Extension/DebugDraw/DebugDrawSubsystem.hpp>
#include <Usagi/Graphics/Game/GraphicalGame.hpp>
#include <Usagi/Graphics/RenderTarget/RenderTargetDescriptor.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Runtime/Input/InputManager.hpp>
#include <Usagi/Runtime/Input/Mouse/Mouse.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Window/Window.hpp>
#include <Usagi/Transform/TransformComponent.hpp>

#include "DebugDrawDemoComponent.hpp"

void usagi::DebugDrawGameState::setupInput()
{
    const auto mouse = mGame->runtime()->inputManager()->virtualMouse();
    mouse->addEventListener(&mInputMap);
    mouse->addEventListener(this);
}

void usagi::DebugDrawGameState::setupCamera()
{
    using namespace std::placeholders;

    mCameraElement = addChild<ModelCameraMan>(
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

usagi::DebugDrawGameState::DebugDrawGameState(
    Element *parent,
    std::string name,
    GraphicalGame *game)
    : GraphicalGameState(parent, std::move(name), game)
{
    // this RenderableSubsystem will be registered by subsystemFilter()
    // of GraphicalGameState
    mDebugDraw = addSubsystem("dd", std::make_unique<DebugDrawSubsystem>(game));
    mDebugDrawRoot = addChild("DebugDrawRoot");
    mDebugDrawRoot->addComponent<DebugDrawDemoComponent>();

    setupInput();
    setupCamera();
}

usagi::DebugDrawGameState::~DebugDrawGameState()
{
    const auto mouse = mGame->runtime()->inputManager()->virtualMouse();
    mouse->removeEventListener(&mInputMap);
    mouse->removeEventListener(this);
}

void usagi::DebugDrawGameState::update(const Clock &clock)
{
    // todo auto set all render sizes
    const auto framebuffer_size = mGame->mainWindow().swapchain->size();
    const auto aspect =
        static_cast<float>(framebuffer_size.x()) / framebuffer_size.y();
    mCameraElement->camera()->setPerspective(
        degreesToRadians(90.f), aspect, 0.5f, 100.f);
    mDebugDraw->setWorldToNDC(
        // world -> camera local -> NDC
        mCameraElement->camera()->localToNDC() *
        mCameraElement->comp<TransformComponent>()->worldToLocal());
    mDebugDraw->setRenderSizes(
        mGame->mainWindow().window->size(),
        framebuffer_size);

    GraphicalGameState::update(clock);
}

void usagi::DebugDrawGameState::onMouseButtonStateChange(
    const MouseButtonEvent &e)
{
    if(e.button == MouseButtonCode::RIGHT)
        e.mouse->setImmersiveMode(e.pressed);
}
