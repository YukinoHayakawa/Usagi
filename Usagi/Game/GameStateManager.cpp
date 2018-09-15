#include "GameStateManager.hpp"

#include "GameState.hpp"

usagi::GameStateManager::GameStateManager(Element *parent, std::string name)
    : Element(parent, std::move(name))
{
}

void usagi::GameStateManager::pushState(GameState *state)
{
    if(mTopState)
        state->mPreviousState = mTopState;
    mTopState = state;
}

void usagi::GameStateManager::changeState(GameState *state)
{
    if(mTopState) popState();
    pushState(state);
}

void usagi::GameStateManager::popState()
{
    assert(mTopState);
    const auto old_top = mTopState;
    mTopState = mTopState->mPreviousState;
    old_top->mPreviousState = nullptr;
}

void usagi::GameStateManager::update(const Clock &clock)
{
    if(mTopState)
    {
        mTopState->update(clock);
    }
    // debug state is likely to gather information from the current state,
    // so update it after top state.
    if(mDebugState)
    {
        mDebugState->update(clock);
    }
}
