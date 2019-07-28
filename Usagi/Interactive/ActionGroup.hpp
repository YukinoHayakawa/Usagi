#pragma once

#include <string>
#include <functional>
#include <map>
#include <set>

#include <Usagi/Core/Element.hpp>
#include <Usagi/Math/Matrix.hpp>
#include <Usagi/Runtime/Input/Keyboard/KeyCode.hpp>
#include <Usagi/Runtime/Input/Mouse/MouseButtonCode.hpp>
#include <Usagi/Runtime/Input/Keyboard/KeyEventListener.hpp>
#include <Usagi/Runtime/Input/Mouse/MouseEventListener.hpp>

namespace usagi
{
// note that although this is a listener of key and mouse events, it is not
// directly registered at the devices. the events are delegated from
// InputMapping.
class ActionGroup
    : public Element
    , public KeyEventListener
    , public MouseEventListener
{
    friend class InputMapping;

public:
    using BinaryActionHandler = std::function<void(bool active)>;
    using AnalogAction1DHandler = std::function<void(float value)>;
    // input handlers typically manipulates the values, thus it is passed
    // by value.
    using AnalogAction2DHandler = std::function<void(Vector2f value)>;

private:
    // todo note that one key can only be bound to one action but mouse movement can be bound to multiple, this should be more consistent.

    std::map<std::string, BinaryActionHandler> mBinaryActions;
    std::map<KeyCode, std::string> mKeyBindings;
    std::map<MouseButtonCode, std::string> mMouseButtonBindings;

    void performBinaryAction(const std::string &name, bool active);

    std::map<std::string, AnalogAction2DHandler> mAnalogActions2D;
    std::set<std::string> mMouseRelativeMoveBindings;

    // using an internally linked list allows more efficient access but is
    // harder to maintain correct. generally there won't be many action groups
    // so doing so does not worth the effort.
    // ActionGroup *mNextActiveGroup = nullptr;
    bool mActive = false;

public:
    ActionGroup(Element *parent, std::string name);

    // todo: unity conceptualizes all actions as 2d axis. which has its benefit but might not suit all needs.

    /*
     * 1D Binary Actions
     */

    void setBinaryActionHandler(std::string name, BinaryActionHandler handler);

    void bindKey(std::string action, KeyCode code);
    void bindMouseButton(std::string action, MouseButtonCode code);

    // void addAnalogAction1D(std::string name, AnalogAction1DHandler handler);

    /*
     * 2D Analog Actions
     */

    void setAnalogAction2DHandler(
        std::string name,
        AnalogAction2DHandler handler);

    /**
     * \brief
     * \param name The name of a 2D analog action.
     */
    void bindMouseRelativeMovement(std::string name);

    /*
     * Handler management
     */

    void removeAllHandlers();
    void activate();
    void deactivate();
    bool isActive() const { return mActive; }

    /*
     * Event Listeners
     */

    bool onMouseMove(const MousePositionEvent &e) override;
    bool onKeyStateChange(const KeyEvent &e) override;
    bool onMouseButtonStateChange(const MouseButtonEvent &e) override;
};
}
