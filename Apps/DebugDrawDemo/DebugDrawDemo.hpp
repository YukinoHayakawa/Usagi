#pragma once

#include <Usagi/Game/Game.hpp>
#include <Usagi/Interactive/InputMap.hpp>
#include <Usagi/Game/SingleWindowGame.hpp>

namespace usagi
{
class PerspectiveCamera;
struct CameraComponent;
struct TransformComponent;
class DebugDrawSubsystem;
class GpuImage;
class ModelViewCameraController;

class DebugDrawDemo
    : public SingleWindowGame
    , public MouseEventListener
{
    std::shared_ptr<GpuImage> mDepthBuffer;
    DebugDrawSubsystem *mDebugDraw = nullptr;
    Element *mDebugDrawRoot = nullptr;
    Element *mCameraElement = nullptr;
    TransformComponent *mCameraTransform = nullptr;
    std::shared_ptr<PerspectiveCamera> mCamera;
    std::unique_ptr<ModelViewCameraController> mCameraController;
    InputMap mInputMap;

    void setupInput();
    void createRenderTargets();
    void setupDebugDraw();
    void setupCamera();

public:
    explicit DebugDrawDemo(Runtime *runtime);
    ~DebugDrawDemo();

    void onMouseButtonStateChange(const MouseButtonEvent &e) override;
    void run();
};
}
