#pragma once

#include <Usagi/Camera/CameraMan.hpp>
#include <Usagi/Game/SingleWindowGame.hpp>
#include <Usagi/Runtime/Input/Mouse/MouseEventListener.hpp>
#include <Usagi/Interactive/InputMap.hpp>

#include "Script/Lua.hpp"

namespace usagi
{
class GpuImage;
class ModelViewCameraController;
class PerspectiveCamera;
}

namespace usagi::moeloop
{
class SortedSpriteRenderingSubsystem;
class Scene;

class MoeLoop
    : public SingleWindowGame
    , public MouseEventListener
{
    Element *mSceneRoot = nullptr;

    std::shared_ptr<GpuImage> mDepthBuffer;
    SortedSpriteRenderingSubsystem *mSpriteRender = nullptr;

    kaguya::State mLuaContext;
    kaguya::LuaThread mCurrentScript;

    using ModelCameraMan =
        CameraMan<PerspectiveCamera, ModelViewCameraController>;
    ModelCameraMan *mCameraElement = nullptr;
    InputMap mInputMap;

    void bindScript();

    Scene * createScene(const std::string &name);
    static void unimplemented(const std::string &msg);
    void continueScript();

    void setupCamera();
    void setupGraphics();
    void createRenderTargets();

public:
    explicit MoeLoop(Runtime *runtime);
    ~MoeLoop();

    void mainLoop();

    void onMouseButtonStateChange(const MouseButtonEvent &e) override;
    void onMouseWheelScroll(const MouseWheelEvent &e) override;
};
}
