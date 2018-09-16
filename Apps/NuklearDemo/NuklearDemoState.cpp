#include "NuklearDemoState.hpp"

#include <Usagi/Graphics/Game/GraphicalGame.hpp>
#include <Usagi/Runtime/Window/Window.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>
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
    mNuklear = addSubsystem("nuklear", std::make_unique<NuklearSubsystem>(
        mGame,
        mGame->mainWindow().window,
        input_manager->virtualKeyboard(),
        input_manager->virtualMouse()
    ));
    addChild("NuklearRoot")->addComponent<NuklearDemoComponent>();
}

void usagi::NuklearDemoState::update(const Clock &clock)
{
    mNuklear->setRenderSizes(
        mGame->mainWindow().window->size(),
        mGame->mainWindow().swapchain->size()
    );

    GraphicalGameState::update(clock);
}
