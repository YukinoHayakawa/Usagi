#pragma once

#include <Usagi/Camera/Controller/CameraMan.hpp>

#include <MoeLoop/Script/Lua.hpp>

#include "MoeLoopGameState.hpp"

namespace usagi
{
class StaticCameraController;
class OrthogonalCamera;
}

namespace usagi::moeloop
{
class Scene;

class SceneState : public MoeLoopGameState
{
    Scene *mScene = nullptr;
    kaguya::LuaThread mSceneThread;
    kaguya::LuaFunction mSceneScript;

    using ModelCameraMan =
        CameraMan<OrthogonalCamera, StaticCameraController>;
    ModelCameraMan *mCameraElement = nullptr;

    void continueScript();

public:
    SceneState(Element *parent, std::string name, MoeLoopGame *game);

    Scene * scene() const { return mScene; }

    void update(const Clock &clock) override;
};
}
