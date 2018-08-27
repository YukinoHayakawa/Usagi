#pragma once

#include <string>
#include <vector>

#include <Usagi/Utility/Noncopyable.hpp>

#include "../Win32.hpp"
#include "../Win32MacroFix.hpp"

namespace usagi
{
class Win32Keyboard;
class Win32Mouse;
class Win32Gamepad;

struct Win32RawInputDeviceEnumeration
{
    std::vector<std::shared_ptr<Win32Keyboard>> keyboards;
    std::vector<std::shared_ptr<Win32Mouse>> mice;
    std::vector<std::shared_ptr<Win32Gamepad>> gamepads;
};

class Win32RawInputDevice : Noncopyable
{
protected:
    HANDLE mDeviceHandle = nullptr;
    std::string mName;
    std::string mInstanceId;

    void checkDevice(RAWINPUT *raw, DWORD type) const;

    static std::string deviceNameFromHandle(HANDLE handle);

    static std::shared_ptr<Win32Keyboard> createKeyboard(
        HANDLE handle,
        const RID_DEVICE_INFO_KEYBOARD &info);
    static std::shared_ptr<Win32Mouse> createMouse(
        HANDLE handle,
        const RID_DEVICE_INFO_MOUSE &info);
    static std::shared_ptr<Win32Gamepad> createGamepad(
        HANDLE handle,
        const RID_DEVICE_INFO_HID &info);

public:
    Win32RawInputDevice(HANDLE device_handle, std::string name);
    virtual ~Win32RawInputDevice() = default;

    virtual void handleRawInput(RAWINPUT *data) = 0;

    static Win32RawInputDeviceEnumeration enumerateDevices();

    HANDLE deviceHandle() const { return mDeviceHandle; }
};
}
