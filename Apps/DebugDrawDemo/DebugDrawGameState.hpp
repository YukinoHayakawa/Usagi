#pragma once

#include <Usagi/Camera/Controller/CameraMan.hpp>
#include <Usagi/Graphics/Game/GraphicalGameState.hpp>
#include <Usagi/Interactive/InputMapping.hpp>

namespace usagi
{
class DebugDrawSubsystem;
class PerspectiveCamera;
class ModelViewCameraController;

class DebugDrawGameState
    : public GraphicalGameState
    , public MouseEventListener
{
    Element *mDebugDrawRoot = nullptr;
    using ModelCameraMan =
        CameraMan<PerspectiveCamera, ModelViewCameraController>;
    ModelCameraMan *mCameraElement = nullptr;
    InputMapping mInputMap;

    void setupInput();
    void setupCamera();

public:
    DebugDrawGameState(Element *parent, std::string name, GraphicalGame *game);
    ~DebugDrawGameState();

    void update(const Clock &clock) override;
    void onMouseButtonStateChange(const MouseButtonEvent &e) override;
};
}
