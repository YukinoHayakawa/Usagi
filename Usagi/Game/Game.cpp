#include "Game.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Window/WindowManager.hpp>
#include <Usagi/Runtime/Input/InputManager.hpp>

#include "GameState.hpp"
#include "GameStateManager.hpp"

usagi::Game::Game(std::shared_ptr<Runtime> runtime)
    : mRuntime(std::move(runtime))
{
    mAssetRoot = mRootElement.addChild<AssetRoot>("Assets");
    mStateManager = mRootElement.addChild<GameStateManager>("States", this);

    mRuntime->initWindow();
    mRuntime->initInput();
}

void usagi::Game::addDeferredAction(DeferredAction action)
{
    mDeferredActions.push_back(std::move(action));
}

void usagi::Game::processInput()
{
    // Process window/input events
    runtime()->windowManager()->processEvents();
    runtime()->inputManager()->processEvents();
}

void usagi::Game::updateClock()
{
    mMasterClock.tick();
}

void usagi::Game::performDeferredActions()
{
    for(auto &&a : mDeferredActions)
    {
        a();
    }
    mDeferredActions.clear();
}

void usagi::Game::frame()
{
    processInput();
    mStateManager->update(mMasterClock);
    performDeferredActions();
    updateClock();
}

void usagi::Game::mainLoop()
{
    while(continueGame())
    {
        frame();
    }
}
