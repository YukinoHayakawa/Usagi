#pragma once

#include <Usagi/Game/Game.hpp>
#include <Usagi/Interactive/InputMap.hpp>
#include <Usagi/Game/GraphicalGame.hpp>
#include <Usagi/Camera/Controller/CameraMan.hpp>

namespace usagi
{
struct TransformComponent;
class DebugDrawSubsystem;
class GpuImage;
class PerspectiveCamera;
class ModelViewCameraController;

class DebugDrawDemo
    : public GraphicalGame
    , public MouseEventListener
{
    std::shared_ptr<GpuImage> mDepthBuffer;
    DebugDrawSubsystem *mDebugDraw = nullptr;
    Element *mDebugDrawRoot = nullptr;
    using ModelCameraMan =
        CameraMan<PerspectiveCamera, ModelViewCameraController>;
    ModelCameraMan *mCameraElement = nullptr;
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
