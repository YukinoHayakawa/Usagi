#include "GameInitState.hpp"

#include <Usagi/Core/Logging.hpp>
#include <MoeLoop/MoeLoopGame.hpp>

namespace usagi::moeloop
{
GameInitState::GameInitState(
    Element *parent,
    std::string name,
    MoeLoopGame *game)
    : MoeLoopGameState(parent, std::move(name), game)
{
    LOG(info, "Loading init.lua");
    // todo load from asset system
    mInit = game->luaContext().loadfile("init.lua");
}

void GameInitState::update(const Clock &clock)
{
    // the init script should bootstrap the engine, load the assets, then
    // switch to other states that is supposed to interact with the player.
    if(mExecuted)
    {
        LOG(error, "Init script does not change to other state.");
        throw std::logic_error("Incorrect logic in init script.");
    }
    mInit();
    mExecuted = true;
}
}
