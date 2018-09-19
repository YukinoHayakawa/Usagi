#pragma once

#include <Usagi/Graphics/Game/GraphicalGameState.hpp>

namespace usagi::moeloop
{
class MoeLoopGame;

class MoeLoopGameState : public GraphicalGameState
{
public:
    MoeLoopGameState(Element *parent, std::string name, MoeLoopGame *game);

    MoeLoopGame * game() const;
};
}
