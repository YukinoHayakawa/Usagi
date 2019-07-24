#include "Game2.hpp"

namespace usagi::ecs
{
Game::~Game()
{
}

void Game::initGame()
{
    executeSystems(mGameInitSystems);
}

void Game::finalizeGame()
{
    executeSystems(mGameFinalizeSystems);
}

void Game::initFrame()
{
    executeSystems(mFrameInitSystems);
}

void Game::updateFrame()
{
    executeSystems(mFrameUpdateSystems);
}

void Game::finalizeFrame()
{
    executeSystems(mFrameFinalizeSystems);
}

bool Game::continueGame()
{
    return true;
}

void Game::frame()
{
    // run frame init systems
    initFrame();
    // run frame update systems
    updateFrame();
    // run frame finalize systems
    finalizeFrame();

    mContext.master_clock.tick();
}

void Game::mainLoop()
{
    // run game init systems
    initGame();
    // loop frames
    while(continueGame())
        frame();
    // run game finalize systems
    finalizeGame();
}

Game & Game::addGameInitSystem(std::unique_ptr<GameInitSystem> sys)
{
    mGameInitSystems.push_back(std::move(sys));
    return *this;
}

Game & Game::addGameFinalizeSystem(std::unique_ptr<GameFinalizeSystem> sys)
{
    mGameFinalizeSystems.push_back(std::move(sys));
    return *this;
}

Game & Game::addFrameInitSystem(std::unique_ptr<FrameInitSystem> sys)
{
    mFrameInitSystems.push_back(std::move(sys));
    return *this;
}

Game & Game::addFrameUpdateSystem(std::unique_ptr<FrameUpdateSystem> sys)
{
    mFrameUpdateSystems.push_back(std::move(sys));
    return *this;
}

Game & Game::addFrameFinalizeSystem(std::unique_ptr<FrameFinalizeSystem> sys)
{
    mFrameFinalizeSystems.push_back(std::move(sys));
    return *this;
}
}
