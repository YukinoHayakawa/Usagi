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
class SortedSpriteRenderingSubsystem;

class SceneState : public MoeLoopGameState
{
    SortedSpriteRenderingSubsystem *mSpriteRenderer = nullptr;

    kaguya::LuaThread mSceneScript;
    using ModelCameraMan =
        CameraMan<OrthogonalCamera, StaticCameraController>;
    ModelCameraMan *mCameraElement = nullptr;

    void continueScript();

public:

    SceneState(
        Element *parent,
        std::string name,
        MoeLoopGame *game,
        kaguya::LuaThread lua_state);

    SceneState(Element *parent, std::string name, MoeLoopGame *game);

    void update(const Clock &clock) override;
};
}
