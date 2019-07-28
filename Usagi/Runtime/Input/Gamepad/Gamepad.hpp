#pragma once

#include <Usagi/Math/Matrix.hpp>
#include <Usagi/Runtime/EventEmitter.hpp>
#include <Usagi/Runtime/Input/Device.hpp>

#include "GamepadButtonCode.hpp"
#include "GamepadEventListener.hpp"

namespace usagi
{
class Gamepad
    : public Device
    , public EventEmitter<GamepadEventListener>
{
public:
    virtual bool isButtonPressed(GamepadButtonCode) = 0;

    /**
     * \brief Get normalized value of left stick position.
     * Value range is [-1, 1]. Left/down is negative, right/up is positive.
     * \return
     */
    virtual Vector2f getLeftStickPosition() const = 0;
    virtual Vector2f getRightStickPosition() const = 0;

    /**
     * \brief Get normalized value of left trigger ranging from 0 to 1.
     * \return
     */
    virtual float getLeftTriggerValue() const = 0;
    virtual float getRightTriggerValue() const = 0;

    /**
     * \brief
     * \param strength Normalized value whose range is [0, 1].
     * \return
     */
    virtual float setStrongMotorStrength(float strength) = 0;
    virtual float setWeakMotorStrength(float strength) = 0;
};
}
