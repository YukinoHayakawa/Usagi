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
class InputMapping
    : public KeyEventListener
    , public MouseEventListener
{
public:
    using BinaryActionHandler = std::function<void(bool active)>;
    using AnalogAction1DHandler = std::function<void(float value)>;
    // input handlers typically manipulates the values, thus it is passed
    // by value.
    using AnalogAction2DHandler = std::function<void(Vector2f value)>;

private:
    std::map<std::string, BinaryActionHandler> mBinaryActions;
    std::map<KeyCode, std::string> mKeyBindings;
    std::map<MouseButtonCode, std::string> mMouseButtonBindings;

    void performBinaryAction(const std::string &name, bool active);

    std::map<std::string, AnalogAction2DHandler> mAnalogActions2D;
    std::set<std::string> mMouseRelativeMoveBindings;

public:
    // --- 1D Binary Actions ---

    void addBinaryAction(std::string name, BinaryActionHandler handler);

    void bindKey(std::string action, KeyCode code);
    void bindMouseButton(std::string action, MouseButtonCode code);

    // void addAnalogAction1D(std::string name, AnalogAction1DHandler handler);

    // --- 2D Analog Actions ---

    void addAnalogAction2D(std::string name, AnalogAction2DHandler handler);

    /**
     * \brief
     * \param name The name of a 2D analog action.
     */
    void bindMouseRelativeMovement(std::string name);

    // --- Event Listeners ---

    void onMouseMove(const MousePositionEvent &e) override;
    void onKeyStateChange(const KeyEvent &e) override;
    void onMouseButtonStateChange(const MouseButtonEvent &e) override;
};
}
