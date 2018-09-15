#pragma once

#include <Usagi/Core/Element.hpp>

namespace usagi
{
class Clock;
class GameState;

class GameStateManager : public Element
{
    GameState *mTopState = nullptr;

    /**
     * \brief Always be the top state.
     */
    GameState *mDebugState = nullptr;

public:
    GameStateManager(Element *parent, std::string name);

    // create state using addChild

    void pushState(GameState *state);
    void changeState(GameState *state);
    void popState();

    void update(const Clock &clock);
};
}
