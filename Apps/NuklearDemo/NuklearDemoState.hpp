#pragma once

#include <Usagi/Graphics/Game/GraphicalGameState.hpp>

namespace usagi
{
class NuklearSubsystem;

class NuklearDemoState : public GraphicalGameState
{
    NuklearSubsystem *mNuklear = nullptr;

public:
    NuklearDemoState(Element *parent, std::string name, GraphicalGame *game);

    void update(const Clock &clock) override;
};
}
