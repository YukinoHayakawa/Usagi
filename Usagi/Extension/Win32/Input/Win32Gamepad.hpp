#pragma once

#include <Usagi/Runtime/Input/Gamepad/Gamepad.hpp>

#include "Win32RawInputDevice.hpp"
#include <hidsdi.h>
#include "../Win32MacroFix.hpp"

namespace usagi
{
class Win32Gamepad
    : public Win32RawInputDevice
    , public Gamepad
{
    std::vector<char> mPreparsedData;
    HIDP_CAPS mCaps { };
    std::vector<HIDP_BUTTON_CAPS> mButtonCaps;
    std::vector<HIDP_VALUE_CAPS> mValueCaps;

    std::vector<bool> mLastButtons, mButtons;
    std::vector<USAGE> mButtonUsages;

public:
    Win32Gamepad(HANDLE device_handle, std::string name);

    std::string name() const override;

    void handleRawInput(RAWINPUT *data) override;

    bool isButtonPressed(GamepadButtonCode) override;

    Vector2f getLeftStickPosition() const override;
    Vector2f getRightStickPosition() const override;

    float getLeftTriggerValue() const override;
    float getRightTriggerValue() const override;

    float setStrongMotorStrength(float strength) override;
    float setWeakMotorStrength(float strength) override;
};
}
