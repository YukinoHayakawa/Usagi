#pragma once

#include <Usagi/Engine/Game/Game.hpp>
#include <Usagi/Engine/Interactive/InputMap.hpp>
#include <Usagi/Engine/Runtime/Window/WindowEventListener.hpp>

namespace usagi
{
class PerspectiveCamera;
struct CameraComponent;
struct TransformComponent;
class Runtime;
class DebugDrawSubsystem;
class GpuImage;
class Swapchain;
class ModelViewCameraController;
class Window;

class DebugDrawDemo
    : public Game
    , public WindowEventListener
    , public MouseEventListener
{
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Swapchain> mSwapchain;
    std::shared_ptr<GpuImage> mDepthBuffer;
    DebugDrawSubsystem *mDebugDraw = nullptr;
    Element *mDebugDrawRoot = nullptr;
    Element *mCameraElement = nullptr;
    TransformComponent *mCameraTransform = nullptr;
    std::shared_ptr<PerspectiveCamera> mCamera;
    std::unique_ptr<ModelViewCameraController> mCameraController;
    InputMap mInputMap;

    void createWindow();
    void setupInput();
    void setupGraphics();
    void createRenderTargets();
    void setupDebugDraw();
    void setupCamera();

public:
    explicit DebugDrawDemo(Runtime *runtime);
    ~DebugDrawDemo();

    void onMouseButtonStateChange(const MouseButtonEvent &e) override;
    void onWindowResizeEnd(const WindowSizeEvent &e) override;
    void run();
};
}
