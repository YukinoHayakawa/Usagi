#include "MoeLoopGame.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Package/Filesystem/FilesystemAssetPackage.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Runtime/Input/InputManager.hpp>
#include <Usagi/Runtime/Input/Mouse/Mouse.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Game/GameStateManager.hpp>

#include "Scene/Scene.hpp"
#include "Scene/Character.hpp"
#include "Scene/ImageLayer.hpp"
#include "Game/GameInitState.hpp"
#include "Game/SceneState.hpp"

namespace usagi::moeloop
{
MoeLoopGame::MoeLoopGame(std::shared_ptr<Runtime> runtime)
    : GraphicalGame(std::move(runtime))
{
    auto mouse = mRuntime->inputManager()->virtualMouse();
    mouse->addEventListener(&mInputMapping);
}

MoeLoopGame::~MoeLoopGame()
{
    auto mouse = mRuntime->inputManager()->virtualMouse();
    mouse->removeEventListener(&mInputMapping);

    // remove all states that may reference this game instance.
    mRootElement.removeChild(mStateManager);
}

void MoeLoopGame::unimplemented(const std::string &msg)
{
    LOG(warn, "Script: unimplemented code: {}", msg);
}

void MoeLoopGame::bindScript()
{
    using namespace std::placeholders;

    mLuaContext.setErrorHandler([](const int error, const char *msg) {
        LOG(error, "Lua error {}: {}", error, msg);
        throw std::runtime_error("Error occurred in script.");
    });

    mLuaContext["unimplemented"].setFunction(&MoeLoopGame::unimplemented);
    mLuaContext["MoeLoop"].setClass(kaguya::UserdataMetatable<MoeLoopGame>()
        .addFunction("addFilesystemPackage", &MoeLoopGame::addFilesystemPackage)
        .addFunction("changeState", &MoeLoopGame::changeState)
        .addFunction("pushState", &MoeLoopGame::pushState)
        .addFunction("popState", &MoeLoopGame::popState)
        .addFunction("createSceneState", &MoeLoopGame::createSceneState)
        .addFunction("currentScene", &MoeLoopGame::currentScene)
    );

    Scene::exportScript(mLuaContext);
    Character::exportScript(mLuaContext);
    ImageLayer::exportScript(mLuaContext);
}

void MoeLoopGame::init()
{
    mStateManager->pushState(mStateManager->addChild<GameInitState>(
        "Init", this));
}

void MoeLoopGame::addFilesystemPackage(
    std::string name,
    const std::string &path)
{
    assets()->addChild<FilesystemAssetPackage>(
        std::move(name), std::filesystem::u8path(path));
}

void MoeLoopGame::changeState(GameState *state)
{
    mStateManager->changeState(state);
}

void MoeLoopGame::pushState(GameState *state)
{
    mStateManager->pushState(state);
}

void MoeLoopGame::popState()
{
    mStateManager->popState();
}

Scene * MoeLoopGame::currentScene() const
{
    if(const auto state = dynamic_cast<SceneState*>(mStateManager->topState()))
    {
        return state->scene();
    }
    return nullptr;
}
}
