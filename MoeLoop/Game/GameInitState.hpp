#pragma once

#include <MoeLoop/Script/Lua.hpp>

#include "MoeLoopGameState.hpp"

namespace usagi::moeloop
{
class GameInitState : public MoeLoopGameState
{
    kaguya::LuaFunction mInit;
    bool mExecuted = false;

public:
    GameInitState(Element *parent, std::string name, MoeLoopGame *game);

    void update(const Clock &clock) override;
};
}
