#include "NuklearDemoState.hpp"

#include <Usagi/Graphics/Game/GraphicalGame.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Input/InputManager.hpp>

#include <Usagi/Extension/Nuklear/NuklearSubsystem.hpp>
#include "NuklearDemoComponent.hpp"

usagi::NuklearDemoState::NuklearDemoState(
    Element *parent,
    std::string name,
    GraphicalGame *game)
    : GraphicalGameState(parent, std::move(name), game)
{
    const auto input_manager = mGame->runtime()->inputManager();
    const auto nk = addSubsystem("nuklear", std::make_unique<NuklearSubsystem>(
        mGame,
        mGame->mainWindow()->window,
        input_manager->virtualKeyboard(),
        input_manager->virtualMouse()
    ));
    nk->setSizeFunctionsFromRenderWindow(mGame->mainWindow());
    addChild("NuklearRoot")->addComponent<NuklearDemoComponent>();
}
