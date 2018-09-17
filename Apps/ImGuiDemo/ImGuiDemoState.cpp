#include "ImGuiDemoState.hpp"

#include <Usagi/Extension/ImGui/ImGuiSubsystem.hpp>
#include <Usagi/Graphics/Game/GraphicalGame.hpp>
#include <Usagi/Runtime/Window/Window.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Input/InputManager.hpp>

#include "ImGuiDemoComponent.hpp"

usagi::ImGuiDemoState::ImGuiDemoState(
    Element *parent,
    std::string name,
    GraphicalGame *game)
    : GraphicalGameState(parent, std::move(name), game)
{
    const auto input_manager = mGame->runtime()->inputManager();
    mImGui = addSubsystem("imgui", std::make_unique<ImGuiSubsystem>(
        mGame,
        mGame->mainWindow().window,
        input_manager->virtualKeyboard(),
        input_manager->virtualMouse()
    ));
    addChild("ImGuiRoot")->addComponent<ImGuiDemoComponent>();
}

void usagi::ImGuiDemoState::update(const Clock &clock)
{
    mImGui->setRenderSizes(
        mGame->mainWindow().window->size(),
        mGame->mainWindow().swapchain->size()
    );

    GraphicalGameState::update(clock);
}
