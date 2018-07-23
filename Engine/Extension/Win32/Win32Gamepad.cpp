#include "Win32Gamepad.hpp"

#pragma comment(lib, "hid.lib")

#include "Win32Helper.hpp"

// https://www.codeproject.com/Articles/185522/Using-the-Raw-Input-API-to-Process-Joystick-Input
usagi::Win32Gamepad::Win32Gamepad(HANDLE device_handle, std::string name)
    : Win32RawInputDevice{ device_handle, std::move(name) }
{
    USHORT caps_length;
    UINT   buffer_size = 0;

    // Get the preparsed data
    if(GetRawInputDeviceInfoW(device_handle, RIDI_PREPARSEDDATA,
        nullptr, &buffer_size) != 0)
        throw win32::Win32Exception("GetRawInputDeviceInfo() failed.");
    mPreparsedData.resize(buffer_size);
    if(GetRawInputDeviceInfo(device_handle, RIDI_PREPARSEDDATA,
        mPreparsedData.data(), &buffer_size) == static_cast<UINT>(-1))
        throw win32::Win32Exception("GetRawInputDeviceInfo() failed.");

    // Get the capabilities
    if(HidP_GetCaps(reinterpret_cast<PHIDP_PREPARSED_DATA>(
        mPreparsedData.data()), &mCaps) != HIDP_STATUS_SUCCESS)
        throw std::runtime_error("HidP_GetCaps() failed.");

    if(mCaps.NumberInputButtonCaps == 0)
        throw std::runtime_error(
            "Found a strange gamepad without any buttons!");
    mButtonCaps.resize(mCaps.NumberInputButtonCaps);
    mValueCaps.resize(mCaps.NumberInputValueCaps);

    caps_length = mCaps.NumberInputButtonCaps;
    if(HidP_GetButtonCaps(HidP_Input, mButtonCaps.data(), &caps_length,
        reinterpret_cast<PHIDP_PREPARSED_DATA>(mPreparsedData.data())) !=
        HIDP_STATUS_SUCCESS)
        throw std::runtime_error("HidP_GetButtonCaps() failed.");
    const auto num_buttons = mButtonCaps[0].Range.UsageMax
        - mButtonCaps[0].Range.UsageMin + 1;
    mButtons.resize(num_buttons);
    mLastButtons.resize(num_buttons);
    mButtonUsages.resize(num_buttons);

    caps_length = mCaps.NumberInputValueCaps;
    if(HidP_GetValueCaps(HidP_Input, mValueCaps.data(), &caps_length,
        reinterpret_cast<PHIDP_PREPARSED_DATA>(mPreparsedData.data())) !=
        HIDP_STATUS_SUCCESS)
        throw std::runtime_error("HidP_GetValueCaps() failed.");
}

std::string usagi::Win32Gamepad::name() const
{
    return mName;
}

void usagi::Win32Gamepad::handleRawInput(RAWINPUT *data)
{
	// Get the pressed buttons
	ULONG usage_length = mButtons.size();
	if(HidP_GetUsages(
		HidP_Input, mButtonCaps[0].UsagePage, 0,
        mButtonUsages.data(), &usage_length,
        reinterpret_cast<PHIDP_PREPARSED_DATA>(mPreparsedData.data()),
        reinterpret_cast<PCHAR>(data->data.hid.bRawData),
        data->data.hid.dwSizeHid
	) != HIDP_STATUS_SUCCESS)
        throw std::runtime_error("HidP_GetUsages() failed.");

    // Reset button states
    std::fill(mButtons.begin(), mButtons.end(), 0);
	for(unsigned i = 0; i < usage_length; i++)
        mButtons[mButtonUsages[i] - mButtonCaps[0].Range.UsageMin] = true;

    for(std::size_t i = 0; i < mLastButtons.size(); ++i)
    {
        if(mButtons[i] != mLastButtons[i])
        {
            GamepadButtonEvent e;
            e.gamepad = this;
            e.code = (GamepadButtonCode)i; // todo
            e.pressed = mButtons[i];
            forEachListener([&](auto h) { h->onButtonStateChange(e); });
        }
    }

    mLastButtons = mButtons;
}

bool usagi::Win32Gamepad::isButtonPressed(GamepadButtonCode)
{
    return { };
}

usagi::Vector2f usagi::Win32Gamepad::getLeftStickPosition() const
{return { };
}

usagi::Vector2f usagi::Win32Gamepad::getRightStickPosition() const
{return { };
}

float usagi::Win32Gamepad::getLeftTriggerValue() const
{return { };
}

float usagi::Win32Gamepad::getRightTriggerValue() const
{return { };
}

float usagi::Win32Gamepad::setStrongMotorStrength(float strength)
{return { };
}

float usagi::Win32Gamepad::setWeakMotorStrength(float strength)
{return { };
}
