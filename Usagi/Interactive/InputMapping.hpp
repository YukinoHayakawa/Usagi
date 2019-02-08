#pragma once

#include <string>
#include <vector>

#include "ActionGroup.hpp"

namespace usagi
{
class InputMapping
    : public Element
    , public KeyEventListener
    , public MouseEventListener
{
    bool acceptChild(Element *child) override;

protected:
    std::vector<ActionGroup> mActionGroups;

public:
    InputMapping(Element *parent, std::string name);

    /*
     * Action Group management.
     *
     * An action group is a set of actions that are coherently together in a
     * game context. Such as the controls for game character or vehicle, or
     * pause menu controls that are active through the most stages of the game.
     * This grouping allow separated controls of input handling and separation
     * of game states and input handling. In a typical game, the action groups
     * shall be defined by the game and states should active certain groups
     * according its gameplay need, or even manage extra temporary action
     * groups.
     *
     * Action group handlers should be cleared before transitioning to another
     * state to prevent leaking input actions to inactive states (states not
     * on the top of the stack). This behavior should be performed by the
     * pause() method of states. The action handlers are correspondingly
     * registered in resume() method and during the creation of the states.
     *
     * Input events are handled in a chain of responsibilities. The most
     * recently added action group has the highest priority of processing
     * input events. If it does not handle the event or is inactive, the event
     * will be passed to the next action group in the reverse order of
     * appending, until the event is handled or the action groups were all
     * considered.
     */

    ActionGroup * addActionGroup(std::string name)
    {
        return addChild<ActionGroup>(std::move(name));
    }

    ActionGroup * actionGroup(const std::string &name) const
    {
        return static_cast<ActionGroup*>(getChild(name));
    }

    void removeActionGroup(const std::string &name)
    {
        return removeChild(name);
    }

    /**
     * \brief Deactivate all action groups and remove all registered action
     * handlers.
     */
    void disableAllActionGroups();

    /*
     * Event Listeners
     */

    bool onMouseMove(const MousePositionEvent &e) override;
    bool onKeyStateChange(const KeyEvent &e) override;
    bool onMouseButtonStateChange(const MouseButtonEvent &e) override;
};
}
