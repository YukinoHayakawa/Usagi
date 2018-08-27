#pragma once

#include <string>
#include <functional>
#include <map>
#include <set>

#include <Usagi/Core/Math.hpp>
#include <Usagi/Runtime/Input/Keyboard/KeyCode.hpp>
#include <Usagi/Runtime/Input/Mouse/MouseButtonCode.hpp>
#include <Usagi/Runtime/Input/Keyboard/KeyEventListener.hpp>
#include <Usagi/Runtime/Input/Mouse/MouseEventListener.hpp>

namespace usagi
{
enum class KeyState
{
    PRESSED = 0b001,
    REPEATED = 0b010,
    RELEASED = 0b100,
    ANY = PRESSED | REPEATED | RELEASED,
};

class InputMap
    : public KeyEventListener
    , public MouseEventListener
{
public:
    using DiscreteActionHandler = std::function<void(bool active)>;
    using AnalogAction1DHandler = std::function<void(float value)>;
    // input handlers typically manipulates the values, thus it is passed
    // by value.
    using AnalogAction2DHandler = std::function<void(Vector2f value)>;

private:
    std::map<std::string, AnalogAction2DHandler> mAnalogActions2D;
    std::set<std::string> mMouseRelativeMoveBindings;

public:
    /*
    void addDiscreteAction(std::string name, DiscreteActionHandler handler);

    void bindKey(
        const std::string &action,
        KeyCode code,
        KeyState state);
    void bindMouseButton(
        const std::string &action,
        MouseButtonCode code,
        KeyState state);

    void addAnalogAction1D(std::string name, AnalogAction1DHandler handler);
    */


    void addAnalogAction2D(std::string name, AnalogAction2DHandler handler);

    /**
     * \brief
     * \param name The name of a 2D analog action.
     */
    void bindMouseRelativeMovement(std::string name);


    void onMouseMove(const MousePositionEvent &e) override;
};
}
