#pragma once

#include <Usagi/Graphics/Game/GraphicalGameState.hpp>

namespace usagi
{
class ImGuiSubsystem;

class ImGuiDemoState : public GraphicalGameState
{
public:
    ImGuiDemoState(Element *parent, std::string name, GraphicalGame *game);
};
}
