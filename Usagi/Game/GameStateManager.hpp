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

    // create state using addChild

    void pushState(GameState *state);
    void changeState(GameState *state);
    void popState();

    void update(const Clock &clock);
};
}
