#pragma once

#include <Usagi/Camera/Controller/CameraMan.hpp>
#include <Usagi/Game/GraphicalGame.hpp>
#include <Usagi/Runtime/Input/Mouse/MouseEventListener.hpp>
#include <Usagi/Interactive/InputMap.hpp>

#include "Script/Lua.hpp"

namespace usagi
{
class GpuImage;
class StaticCameraController;
class OrthogonalCamera;
}

namespace usagi::moeloop
{
class SortedSpriteRenderingSubsystem;
class Scene;

class MoeLoop
    : public GraphicalGame
    , public MouseEventListener
{
    std::shared_ptr<GpuImage> mDepthBuffer;
    SortedSpriteRenderingSubsystem *mSpriteRender = nullptr;

    kaguya::State mLuaContext;
    kaguya::LuaThread mCurrentScript;

    using ModelCameraMan =
        CameraMan<OrthogonalCamera, StaticCameraController>;
    ModelCameraMan *mCameraElement = nullptr;
    InputMap mInputMap;

    void bindScript();

    Element *mSceneRoot = nullptr;
    Scene *mCurrentScene = nullptr;

    void addFilesystemPackage(std::string name, const std::string &path);
    Scene * loadScene(const std::string &name);
    void setCurrentScene(Scene *scene);
    static void unimplemented(const std::string &msg);
    void continueScript();

    void setupCamera();
    void setupGraphics();
    void createRenderTargets();

public:
    explicit MoeLoop(Runtime *runtime);
    ~MoeLoop();

    void mainLoop();

    void onWindowResizeEnd(const WindowSizeEvent &e) override;
    void onMouseButtonStateChange(const MouseButtonEvent &e) override;
    void onMouseWheelScroll(const MouseWheelEvent &e) override;
};
}
