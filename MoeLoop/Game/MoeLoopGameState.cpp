#include "MoeLoopGameState.hpp"

#include <MoeLoop/MoeLoopGame.hpp>

namespace usagi::moeloop
{
MoeLoopGameState::MoeLoopGameState(
    Element *parent,
    std::string name,
    MoeLoopGame *game)
    : GraphicalGameState(parent, std::move(name), game)
{
}

MoeLoopGame * MoeLoopGameState::game() const
{
    return static_cast<MoeLoopGame*>(mGame);
}
}
