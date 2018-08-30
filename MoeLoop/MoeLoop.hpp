#pragma once

#include <Usagi/Camera/CameraMan.hpp>
#include <Usagi/Game/SingleWindowGame.hpp>
#include <Usagi/Runtime/Input/Mouse/MouseEventListener.hpp>

#include "Script/Lua.hpp"

namespace usagi::moeloop
{
class Scene;
class ModelViewCameraController;
class PerspectiveCamera;

class MoeLoop
    : public SingleWindowGame
    , public MouseEventListener
{
    kaguya::State mLuaContext;
    kaguya::LuaThread mCurrentScript;

    using ModelCameraMan =
        CameraMan<PerspectiveCamera, ModelViewCameraController>;
    ModelCameraMan *mCameraElement = nullptr;

    void bindScript();

    Scene * createScene(
        const std::string &name,
        std::uint32_t width,
        std::uint32_t height);
    static void unimplemented(const std::string &msg);
    void continueScript();

public:
    explicit MoeLoop(Runtime *runtime);
    ~MoeLoop();

    void mainLoop();

    void onMouseButtonStateChange(const MouseButtonEvent &e) override;
    void onMouseWheelScroll(const MouseWheelEvent &e) override;
};
}
