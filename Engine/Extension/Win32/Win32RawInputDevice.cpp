#include "Win32RawInputDevice.hpp"

#include <cassert>

#include <Usagi/Engine/Core/Logging.hpp>
#include <Usagi/Engine/Utility/String.hpp>

#include "Win32Platform.hpp"
#include "Win32Mouse.hpp"
#include "Win32Gamepad.hpp"
#include "Win32Keyboard.hpp"

void usagi::Win32RawInputDevice::checkDevice(RAWINPUT *raw, DWORD type) const
{
    assert(raw->header.dwType == type);
    // if mDeviceHandle is nullptr, messages from any device of the same
    // type are processed.
    if(mDeviceHandle != nullptr)
        assert(raw->header.hDevice == mDeviceHandle);
}

usagi::Win32RawInputDevice::Win32RawInputDevice(
    HANDLE device_handle,
    std::string name)
    : mDeviceHandle { device_handle }
    , mName { std::move(name) }
{
}

void usagi::Win32RawInputDevice::handleRawInput(RAWINPUT *data)
{
}

usagi::Win32RawInputDeviceEnumeration usagi::Win32RawInputDevice::
    enumerateDevices()
{
    LOG(info, "Enumerating raw input devices");

    Win32RawInputDeviceEnumeration devices;

    // get device list
    std::vector<RAWINPUTDEVICELIST> raw_input_device_list;
    {
        UINT num_devices;
        if(GetRawInputDeviceList(
            nullptr, &num_devices, sizeof(RAWINPUTDEVICELIST)) != 0)
            throw std::runtime_error("Could not get raw input device amount.");
        raw_input_device_list.resize(num_devices);
        if(GetRawInputDeviceList(raw_input_device_list.data(), &num_devices,
            sizeof(RAWINPUTDEVICELIST)) == static_cast<unsigned>(-1))
            throw std::runtime_error("Could not get raw input device list.");
    }

    for(auto &&device : raw_input_device_list)
    {
        // get device path as kernal object
        UINT size;
        std::wstring path = L"<Error getting device path>";
        if(GetRawInputDeviceInfo(device.hDevice, RIDI_DEVICENAME,
            nullptr, &size) == 0)
        {
            path.resize(size, L'\0');
            GetRawInputDeviceInfo(device.hDevice, RIDI_DEVICENAME,
                path.data(), &size);
        }

        // get device info
        RID_DEVICE_INFO info { sizeof(RID_DEVICE_INFO) };
        size = info.cbSize;
        GetRawInputDeviceInfo(device.hDevice, RIDI_DEVICEINFO,
            &info, &size);

        LOG(info, "Path:              %ls", ws2s(path));
        //LOG(info, "Friendly Name:          %ls", device_name.c_str());

        switch(info.dwType)
        {
            case RIM_TYPEMOUSE:
            {
                devices.mice.push_back(createMouse(
                    device.hDevice, info.mouse));
                break;
            }
            case RIM_TYPEKEYBOARD:
            {
                devices.keyboards.push_back(createKeyboard(
                    device.hDevice, info.keyboard));
                break;
            }
            case RIM_TYPEHID:
            {
                // gamepad
                if(info.hid.usUsagePage == 0x01 && info.hid.usUsage == 0x05)
                {
                    devices.gamepads.push_back(createGamepad(
                        device.hDevice, info.hid));
                    HANDLE hid_device = CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                        OPEN_EXISTING, 0, NULL);
                    auto i = GetLastError();
                    assert(hid_device != INVALID_HANDLE_VALUE);
                    uint8_t buf[32];
                    DWORD bytes_written;
                    ReadFile(hid_device, buf, 32, &bytes_written, nullptr);
                    i = GetLastError();
                    memset(buf, 0, sizeof(buf));
                    buf[0] = 0x05;
                    buf[1] = 0xFF;
                    buf[4] = 255;  // 0-255
                    buf[5] = 255;   // 0-255
                    buf[6] = 0;         // 0-255
                    buf[7] = 255;       // 0-255
                    buf[8] = 0;        // 0-255
                    WriteFile(hid_device, buf, sizeof(buf), &bytes_written, NULL);
                    i = GetLastError();
                    assert(bytes_written == 32);

                }

                break;
            }
            default:
            {
                // ignore other devices
                break;
            }
        }
    }

    return devices;
}

std::string usagi::Win32RawInputDevice::deviceNameFromHandle(HANDLE handle)
{
    return { };
}

std::shared_ptr<usagi::Win32Keyboard>
    usagi::Win32RawInputDevice::createKeyboard(
        HANDLE handle,
        const RID_DEVICE_INFO_KEYBOARD &info)
{
    LOG(info, "Device Type        : Keyboard");
    LOG(info, "Keyboard Mode      : {}", info.dwKeyboardMode);
    LOG(info, "# of function keys : {}", info.dwNumberOfFunctionKeys);
    LOG(info, "# of indicators    : {}", info.dwNumberOfIndicators);
    LOG(info, "# of keys total    : {}", info.dwNumberOfKeysTotal);
    LOG(info, "Type               : {}", info.dwType);
    LOG(info, "Subtype            : {}", info.dwSubType);

    return std::make_shared<Win32Keyboard>(
        handle, deviceNameFromHandle(handle));
}

std::shared_ptr<usagi::Win32Mouse> usagi::Win32RawInputDevice::createMouse(
    HANDLE handle,
    const RID_DEVICE_INFO_MOUSE &info)
{
    LOG(info, "Device Type        : Mouse");
    LOG(info, "Id                 : {}", info.dwId);
    LOG(info, "# of Buttons       : {}", info.dwNumberOfButtons);
    LOG(info, "Sample rate        : {}", info.dwSampleRate);

    return std::make_shared<Win32Mouse>(
        handle, deviceNameFromHandle(handle));
}

std::shared_ptr<usagi::Win32Gamepad> usagi::Win32RawInputDevice::createGamepad(
    HANDLE handle,
    const RID_DEVICE_INFO_HID &info)
{
    LOG(info, "Device Type        : HID");
    LOG(info, "Vendor Id          : {}", info.dwVendorId);
    LOG(info, "Product Id         : {}", info.dwProductId);
    LOG(info, "Version No         : {}", info.dwVersionNumber);
    LOG(info, "Usage Page         : {}", info.usUsagePage);
    LOG(info, "Usage              : {}", info.usUsage);

    return std::make_shared<Win32Gamepad>(
        handle, deviceNameFromHandle(handle));
}
