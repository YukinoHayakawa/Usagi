#include "GameStateManager.hpp"

#include <cassert>

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

void usagi::GameStateManager::pushState(GameState *state, bool pause_below)
{
    // the state should not be on the stack already
    assert(state->mPreviousState == nullptr);
    // changing state during updating process may lead to unexpected
    // behaviors, therefore they should be delayed to the end of update.
    // however, this may also have side effects such as preventing quick
    // input sequence involving changing states as the current state will
    // be consuming too many input events. this generally won't be a problem.
    if(mUpdating)
    {
        LOG(info, "Creating deferred pushState action: {}", state->name());
        mGame->addDeferredAction([=]() {
            assert(!mUpdating);
            pushState(state, pause_below);
        });
    }
    else
    {
        LOG(info, "pushState: {}", state->name());
        // chain the states
        state->mPreviousState = mTopState;
        if(mTopState && pause_below)
            mTopState->pause();
        // push state
        mTopState = state;
    }
}

void usagi::GameStateManager::changeState(GameState *state)
{
    assert(state->mPreviousState == nullptr);
    if(mUpdating)
    {
        LOG(info, "Creating deferred changeState action: {}", state->name());
        mGame->addDeferredAction([=]() {
            assert(!mUpdating);
            changeState(state);
        });
    }
    else
    {
        LOG(info, "changeState: {}", state->name());
        if(mTopState) popState(false);
        pushState(state, false);
    }
}

void usagi::GameStateManager::popState(bool resume_below)
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
        if(mTopState && resume_below)
            mTopState->resume();
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
        // if the top state wants, it can update the states below it
        // through the linked list.
        mTopState->update(clock);
    }
    // debug state is likely to gather information from the current state,
    // so update it after top state.
    if(mDebugState)
    {
        mDebugState->update(clock);
    }
}
