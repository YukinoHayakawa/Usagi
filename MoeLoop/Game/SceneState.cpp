#include "SceneState.hpp"

#include <Usagi/Animation/AnimationSubsystem.hpp>
#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Converter/Uncached/StringAssetConverter.hpp>
#include <Usagi/Camera/Controller/StaticCameraController.hpp>
#include <Usagi/Camera/OrthogonalCamera.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Graphics/Game/GraphicalGame.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Transform/TransformComponent.hpp>

#include <MoeLoop/JSON/JsonPropertySheetAssetConverter.hpp>
#include <MoeLoop/MoeLoopGame.hpp>
#include <MoeLoop/Render/SortedSpriteRenderingSubsystem.hpp>
#include <MoeLoop/Render/SpriteComponent.hpp>
#include <MoeLoop/Scene/Scene.hpp>

namespace usagi::moeloop
{
void SceneState::loadScene()
{
    LOG(info, "Loading scene: {}", name());

    mScene = addChild<Scene>(name(), game()->runtime(), game()->assets());
    mScene->load(game()->assets()->uncachedRes<JsonPropertySheetAssetConverter>(
        fmt::format("scenes/{}.json", name())
    ));
    mSceneThread = game()->luaContext()->newThread();
    mSceneScript = game()->luaContext()->loadstring(
        game()->assets()->uncachedRes<StringAssetConverter>(
            fmt::format("scripts/{}.lua", name())
        ));
    mSceneThread.setFunction(mSceneScript);
}

void SceneState::createCamera()
{
    mCameraElement = addChild<ModelCameraMan>(
        "CameraMan",
        std::make_shared<OrthogonalCamera>(),
        std::make_shared<StaticCameraController>());
}

void SceneState::setupRenderer()
{
    const auto sprite = addSubsystem(
        "sprite",
        std::make_unique<SortedSpriteRenderingSubsystem>(
            game(), [](TransformComponent *lt, SpriteComponent *ls,
            TransformComponent *rt, SpriteComponent *rs) {
                return lt->position().y() > rt->position().y();
            }
        ));
    sprite->setWorldToNdcFunc([=]() {
        // world -> camera local -> NDC
        return mCameraElement->camera()->localToNDC() *
            mCameraElement->comp<TransformComponent>()->worldToLocal();
    });
}

void SceneState::setupAnimation()
{
    mAnimation = addSubsystem(
        "animation",
        std::make_unique<AnimationSubsystem>()
    );
}

void SceneState::setupInput()
{
    using namespace std::placeholders;
    const auto im = game()->inputMapping();
    im->addBinaryAction("NextMessage",
        std::bind(&SceneState::playerContinueScript, this, _1));
    im->bindKey("NextMessage", KeyCode::ENTER);
    // todo should not be here
}

SceneState::SceneState(Element *parent, std::string name, MoeLoopGame *game)
    : MoeLoopGameState(parent, name, game)
{
    loadScene();
    createCamera();
    // animation system must be inserted before rendering to create correct
    // result.
    setupAnimation();
    setupRenderer();
    setupInput();
}

void SceneState::continueScript()
{
    if(!mSceneThread.isThreadDead())
        mSceneThread.resume<std::string>();
}

void SceneState::update(const Clock &clock)
{
    const Vector2f size = mGame->mainWindow()->swapchain->size().cast<float>();
    mCameraElement->camera()->setOrthogonal(
        0, size.x(), 0, size.y(), -100, 100);

    MoeLoopGameState::update(clock);
}

void SceneState::playerContinueScript(const bool yes)
{
    if(yes)
    {
        if(mAnimation->activeCount() > 0)
        {
        }
        else
        {
            continueScript();
        }
    }
}
}
