#pragma once

#include <Usagi/Core/Element.hpp>

namespace usagi
{
class Game;
class Clock;
class GameState;

class GameStateManager : public Element
{
    Game *mGame = nullptr;
    bool mUpdating = false;

    GameState *mTopState = nullptr;

    /**
     * \brief Always be the top state.
     */
    GameState *mDebugState = nullptr;

public:
    GameStateManager(Element *parent, std::string name, Game *game);

    // the pushed state must be a child created from this manager element.
    // when a state is popped, it is also removed from the children list.
    // the pushed state will be chained in a linked-list. therefore,
    // when an event is broadcast at this manager element, all states will
    // receive the event.
    // todo event broadcasting not implemented

    // these two are used mainly in scripts. the states must be direct children
    // of this state manager.

    void pushState(GameState *state, bool pause_below = true);
    void changeState(GameState *state);

    template <typename State, bool PauseBelow = true, typename... Args>
    void pushState(Args &&... args)
    {
        static_assert(std::is_base_of_v<GameState, State>);
        auto state = addChild<State>(std::forward<Args>(args)...);
        pushState(state, PauseBelow);
    }

    template <typename State, typename... Args>
    void changeState(Args &&... args)
    {
        static_assert(std::is_base_of_v<GameState, State>);
        auto state = addChild<State>(std::forward<Args>(args)...);
        changeState(state);
    }

    void popState(bool resume_below = true);
    GameState *topState() const { return mTopState; }

    void update(const Clock &clock);
};
}
