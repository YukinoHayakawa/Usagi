#include "Win32Gamepad.hpp"

#pragma comment(lib, "hid.lib")

#include <Usagi/Core/Logging.hpp>

#include "../Win32Helper.hpp"

// https://www.codeproject.com/Articles/185522/Using-the-Raw-Input-API-to-Process-Joystick-Input
usagi::Win32Gamepad::Win32Gamepad(HANDLE device_handle, std::string name)
    : Win32RawInputDevice { device_handle, std::move(name) }
{
    USHORT caps_length;
    UINT   buffer_size = 0;

    // Get the preparsed data
    if(GetRawInputDeviceInfoW(device_handle, RIDI_PREPARSEDDATA,
        nullptr, &buffer_size) != 0)
        USAGI_THROW(win32::Win32Exception("GetRawInputDeviceInfo() failed."));
    mPreparsedData.resize(buffer_size);
    if(GetRawInputDeviceInfo(device_handle, RIDI_PREPARSEDDATA,
        mPreparsedData.data(), &buffer_size) == static_cast<UINT>(-1))
        USAGI_THROW(win32::Win32Exception("GetRawInputDeviceInfo() failed."));

    // Get the capabilities
    if(HidP_GetCaps(reinterpret_cast<PHIDP_PREPARSED_DATA>(
        mPreparsedData.data()), &mCaps) != HIDP_STATUS_SUCCESS)
        USAGI_THROW(std::runtime_error("HidP_GetCaps() failed."));

    if(mCaps.NumberInputButtonCaps == 0)
        USAGI_THROW(std::runtime_error(
            "Found a strange gamepad without any buttons!"));
    mButtonCaps.resize(mCaps.NumberInputButtonCaps);
    mValueCaps.resize(mCaps.NumberInputValueCaps);

    caps_length = mCaps.NumberInputButtonCaps;
    if(HidP_GetButtonCaps(HidP_Input, mButtonCaps.data(), &caps_length,
        reinterpret_cast<PHIDP_PREPARSED_DATA>(mPreparsedData.data())) !=
        HIDP_STATUS_SUCCESS)
        USAGI_THROW(std::runtime_error("HidP_GetButtonCaps() failed."));
    const auto num_buttons = mButtonCaps[0].Range.UsageMax
        - mButtonCaps[0].Range.UsageMin + 1;
    mButtons.resize(num_buttons);
    mLastButtons.resize(num_buttons);
    mButtonUsages.resize(num_buttons);

    caps_length = mCaps.NumberInputValueCaps;
    if(HidP_GetValueCaps(HidP_Input, mValueCaps.data(), &caps_length,
        reinterpret_cast<PHIDP_PREPARSED_DATA>(mPreparsedData.data())) !=
        HIDP_STATUS_SUCCESS)
        USAGI_THROW(std::runtime_error("HidP_GetValueCaps() failed."));
}

std::string usagi::Win32Gamepad::name() const
{
    return mName;
}

void usagi::Win32Gamepad::handleRawInput(RAWINPUT *data)
{
    // Get the pressed buttons
    auto usage_length = static_cast<ULONG>(mButtons.size());
    auto result = HidP_GetUsages(
        HidP_Input, mButtonCaps[0].UsagePage, 0,
        mButtonUsages.data(), &usage_length,
        reinterpret_cast<PHIDP_PREPARSED_DATA>(mPreparsedData.data()),
        reinterpret_cast<PCHAR>(data->data.hid.bRawData),
        data->data.hid.dwSizeHid
    );
    if(result != HIDP_STATUS_SUCCESS)
        USAGI_THROW(std::runtime_error("HidP_GetUsages() failed."));

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

    // Get the state of discrete-valued-controls
    ULONG value;
    for(unsigned i = 0; i < mCaps.NumberInputValueCaps; ++i)
    {
        result = HidP_GetUsageValue(
            HidP_Input, mValueCaps[i].UsagePage, 0,
            mValueCaps[i].NotRange.Usage, &value,
            reinterpret_cast<PHIDP_PREPARSED_DATA>(mPreparsedData.data()),
            reinterpret_cast<PCHAR>(data->data.hid.bRawData),
            data->data.hid.dwSizeHid
        );
        switch(result)
        {
            case HIDP_STATUS_SUCCESS:
            // value not found in the report
            case HIDP_STATUS_INCOMPATIBLE_REPORT_ID:
                break;
            default:
                USAGI_THROW(std::runtime_error("HidP_GetUsageValue() failed."));
        }
//        if(mValueCaps[i].NotRange.Usage == HID_USAGE_GENERIC_HATSWITCH)
//        {
//            LOG(info, "{}", value);
////            HidP_SetUsageValue(HidP_Output, mValueCaps[value + 7].UsagePage, 0,
////                mValueCaps[value + 7].NotRange.Usage, 0xFF,
////                reinterpret_cast<PHIDP_PREPARSED_DATA>(mPreparsedData.data()),
////                reinterpret_cast<PCHAR>(data->data.hid.bRawData),
////                data->data.hid.dwSizeHid
////            );
////            HANDLE hid_device = CreateFileW(
////L"\\\\?\\hid#{00001124-0000-1000-8000-00805f9b34fb}_vid&0002054c_pid&09cc#8&30cdc2d8&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}",
////GENERIC_READ | GENERIC_WRITE,
////                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
////                OPEN_EXISTING, 0, NULL);
////            assert(hid_device != INVALID_HANDLE_VALUE);
////
////            uint8_t outputReportBuffer[78] { };
////            outputReportBuffer[0] = 0x11;
////            outputReportBuffer[1] = 0x80;
////            outputReportBuffer[3] = 0xff;
////            outputReportBuffer[6] = 0; //fast motor
////            outputReportBuffer[7] = 0; //slow motor
////            outputReportBuffer[8] = 0xff; //red
////            outputReportBuffer[9] = 0xff; //green
////            outputReportBuffer[10] = 0; //blue
////DWORD bytes_written;
////            assert(HidD_SetOutputReport(hid_device, outputReportBuffer, sizeof(outputReportBuffer)));
////           // assert(WriteFile(hid_device, outputReportBuffer, sizeof(outputReportBuffer), &bytes_written, NULL));
////            auto xxx = GetLastError();
////
//////            assert(bytes_written == 78);
////            CloseHandle(hid_device);
////            if(!result)
////                DebugBreak();
//        }
    }
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
