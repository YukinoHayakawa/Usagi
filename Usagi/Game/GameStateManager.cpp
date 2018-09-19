#include "GameStateManager.hpp"

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Game/Game.hpp>
#include <Usagi/Utility/RAIIHelper.hpp>

#include "GameState.hpp"

usagi::GameStateManager::GameStateManager(
    Element *parent,
    std::string name,
    Game *game)
    : Element(parent, std::move(name))
    , mGame(game)
{
}

void usagi::GameStateManager::pushState(GameState *state)
{
    // the state should not be on the stack already
    assert(state->mPreviousState == nullptr);
    if(mUpdating)
    {
        LOG(info, "Deferred pushState: {}", state->name());
        mGame->addDeferredAction([=]() {
            assert(!mUpdating);
            pushState(state);
        });
    }
    else
    {
        LOG(info, "pushState: {}", state->name());
        if(mTopState)
            state->mPreviousState = mTopState;
        mTopState = state;
    }
}

void usagi::GameStateManager::changeState(GameState *state)
{
    assert(state->mPreviousState == nullptr);
    if(mUpdating)
    {
        LOG(info, "Deferred changeState: {}", state->name());
        mGame->addDeferredAction([=]() {
            assert(!mUpdating);
            changeState(state);
        });
    }
    else
    {
        LOG(info, "changeState: {}", state->name());
        if(mTopState) popState();
        pushState(state);
    }
}

void usagi::GameStateManager::popState()
{
    assert(mTopState);
    if(mUpdating)
    {
        LOG(info, "Deferred popState: {}", mTopState->name());
        mGame->addDeferredAction([=]() {
            assert(!mUpdating);
            popState();
        });
    }
    else
    {
        LOG(info, "popState: {}", mTopState->name());
        const auto old_top = mTopState;
        mTopState = mTopState->mPreviousState;
        old_top->mPreviousState = nullptr;
        removeChild(old_top);
    }
}

void usagi::GameStateManager::update(const Clock &clock)
{
    RAIIHelper update_lock {
        [&]() { mUpdating = true; },
        [&]() { mUpdating = false; }
    };

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
