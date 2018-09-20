#include "ImGuiDemoState.hpp"

#include <Usagi/Graphics/Game/GraphicalGame.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Input/InputManager.hpp>

#include <Usagi/Extension/ImGui/ImGui.hpp>
#include <Usagi/Extension/ImGui/ImGuiSubsystem.hpp>
#include <Usagi/Extension/ImGui/DelegatedImGuiComponent.hpp>

usagi::ImGuiDemoState::ImGuiDemoState(
    Element *parent,
    std::string name,
    GraphicalGame *game)
    : GraphicalGameState(parent, std::move(name), game)
{
    const auto input_manager = mGame->runtime()->inputManager();
    const auto imgui = addSubsystem("imgui", std::make_unique<ImGuiSubsystem>(
        mGame,
        mGame->mainWindow()->window,
        input_manager->virtualKeyboard(),
        input_manager->virtualMouse()
    ));
    imgui->setSizeFunctionsFromRenderWindow(mGame->mainWindow());
    addChild("ImGuiRoot")->addComponent<DelegatedImGuiComponent>(
        [](const Clock &clock) {
            ImGui::ShowDemoWindow();
            ImGui::ShowMetricsWindow();
        });
}
