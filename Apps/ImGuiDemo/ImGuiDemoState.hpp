#pragma once

#include <Usagi/Graphics/Game/GraphicalGameState.hpp>

namespace usagi
{
class ImGuiSubsystem;

class ImGuiDemoState : public GraphicalGameState
{
    ImGuiSubsystem *mImGui = nullptr;

public:
    ImGuiDemoState(Element *parent, std::string name, GraphicalGame *game);

    void update(const Clock &clock) override;
};
}
