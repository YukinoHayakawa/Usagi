#include "SceneState.hpp"

#include <Usagi/Animation/AnimationSubsystem.hpp>
#include <Usagi/Camera/Controller/StaticCameraController.hpp>
#include <Usagi/Camera/OrthogonalCamera.hpp>
#include <Usagi/Graphics/Game/GraphicalGame.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Transform/TransformComponent.hpp>

#include <MoeLoop/MoeLoopGame.hpp>
#include <MoeLoop/Render/SortedSpriteRenderingSubsystem.hpp>
#include <MoeLoop/Render/SpriteComponent.hpp>

namespace usagi::moeloop
{
SceneState::SceneState(
    Element *parent,
    std::string name,
    MoeLoopGame *game,
    kaguya::LuaThread lua_state)
    : MoeLoopGameState(parent, std::move(name), game)
    , mSceneScript(std::move(lua_state))
{
    mCameraElement = addChild<ModelCameraMan>(
        "CameraMan",
        std::make_shared<OrthogonalCamera>(),
        std::make_shared<StaticCameraController>());

    mSpriteRenderer = addSubsystem(
        "sprite",
        std::make_unique<SortedSpriteRenderingSubsystem>(
            game, [](TransformComponent *lt, SpriteComponent *ls,
            TransformComponent *rt, SpriteComponent *rs) {
            return lt->position().y() > rt->position().y();
        }
    ));
    addSubsystem("animation", std::make_unique<AnimationSubsystem>());
}

void SceneState::continueScript()
{
    mSceneScript.resume<std::string>();
}

void SceneState::update(const Clock &clock)
{
    /*const auto size = mGame->mainWindow().swapchain->size();
    mCameraElement->camera()->setOrthogonal(
        0, size.x(), 0, size.y(), -100, 100);
    mSpriteRenderer->setWorldToNDC(
        // world -> camera local -> NDC
        mCameraElement->camera()->localToNDC() *
        mCameraElement->comp<TransformComponent>()->worldToLocal());*/
}
}
