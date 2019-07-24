#pragma once

#include <memory>
#include <vector>

#include "System.hpp"
#include "GameContext.hpp"

namespace usagi::ecs
{
class Game
{
    std::vector<std::unique_ptr<GameInitSystem>> mGameInitSystems;
    std::vector<std::unique_ptr<GameFinalizeSystem>> mGameFinalizeSystems;
    std::vector<std::unique_ptr<FrameInitSystem>> mFrameInitSystems;
    std::vector<std::unique_ptr<FrameUpdateSystem>> mFrameUpdateSystems;
    std::vector<std::unique_ptr<FrameFinalizeSystem>> mFrameFinalizeSystems;

    GameContext mContext;

    template <typename System>
    void executeSystems(std::vector<std::unique_ptr<System>>& systems)
    {
        for(auto &&s : systems)
            s->execute(mContext);
    }

public:
    virtual ~Game();

    void initGame();
    bool continueGame();
    void finalizeGame();

    void initFrame();
    void updateFrame();
    void finalizeFrame();
    void frame();

    void mainLoop();

    Game & addGameInitSystem(std::unique_ptr<GameInitSystem> sys);
    Game & addGameFinalizeSystem(std::unique_ptr<GameFinalizeSystem> sys);
    Game & addFrameInitSystem(std::unique_ptr<FrameInitSystem> sys);
    Game & addFrameUpdateSystem(std::unique_ptr<FrameUpdateSystem> sys);
    Game & addFrameFinalizeSystem(std::unique_ptr<FrameFinalizeSystem> sys);

    GameContext & context()
    {
        return mContext;
    }
};
}
