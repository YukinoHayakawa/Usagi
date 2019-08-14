#include "Win32RawInputDevice.hpp"

#include <cassert>

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Utility/Unicode.hpp>

#include "../Win32Helper.hpp"
#include "Win32InputManager.hpp"
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

usagi::Win32RawInputDeviceEnumeration usagi::Win32RawInputDevice::
    enumerateDevices()
{
    LOG(info, "Raw input devices");
    LOG(info, "--------------------------------");

    Win32RawInputDeviceEnumeration devices;

    // get device list
    std::vector<RAWINPUTDEVICELIST> raw_input_device_list;
    {
        UINT num_devices;
        if(GetRawInputDeviceList(
            nullptr, &num_devices, sizeof(RAWINPUTDEVICELIST)) != 0)
            USAGI_THROW(win32::Win32Exception("GetRawInputDeviceList() failed."));
        raw_input_device_list.resize(num_devices);
        if(GetRawInputDeviceList(raw_input_device_list.data(), &num_devices,
            sizeof(RAWINPUTDEVICELIST)) == static_cast<unsigned>(-1))
            USAGI_THROW(win32::Win32Exception("GetRawInputDeviceList() failed."));
    }

    for(auto &&device : raw_input_device_list)
    {
        // get DOS device path
        // https://docs.microsoft.com/en-us/dotnet/standard/io/file-path-formats#dos-device-paths
        UINT size;
        std::wstring path = L"<Error getting device path>";
        if(GetRawInputDeviceInfoW(device.hDevice, RIDI_DEVICENAME,
            nullptr, &size) == 0)
        {
            path.resize(size, L'\0');
            GetRawInputDeviceInfoW(device.hDevice, RIDI_DEVICENAME,
                path.data(), &size);
        }

        // get device info
        RID_DEVICE_INFO info { sizeof(RID_DEVICE_INFO) };
        size = info.cbSize;
        GetRawInputDeviceInfoW(device.hDevice, RIDI_DEVICEINFO,
            &info, &size);

        LOG(info, "Path               : {}", utf16To8(path));

        bool got_path = false;
        if(path.length() > 4)
        {
            // translate from Win32 namespace to NT namespace
            // https://docs.microsoft.com/en-us/windows/desktop/fileio/naming-a-file#namespaces
            // https://superuser.com/questions/884347/win32-and-the-global-namespace
            path = L"\\GLOBAL??" + path.substr(3);
            const auto dev_obj_name =
                utf16To8(win32::resolveNtSymbolicLink(path));
            if(!dev_obj_name.empty())
            {
                got_path = true;
                LOG(info, "Name               : {}",
                    Win32InputManager::deviceFriendlyName(dev_obj_name));
            }
        }
        if(!got_path)
        {
            LOG(info, "Name               : <Invalid device path>");
        }

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
                LOG(info, "Device Type        : HID");
                LOG(info, "Vendor Id          : {}", info.hid.dwVendorId);
                LOG(info, "Product Id         : {}", info.hid.dwProductId);
                LOG(info, "Version No         : {}", info.hid.dwVersionNumber);
                LOG(info, "Usage Page         : {}", info.hid.usUsagePage);
                LOG(info, "Usage              : {}", info.hid.usUsage);
                // gamepad
                if(info.hid.usUsagePage == 0x01 && info.hid.usUsage == 0x05)
                {
                    devices.gamepads.push_back(createGamepad(
                        device.hDevice, info.hid));

                }

                break;
            }
            default:
            {
                // ignore other devices
                break;
            }
        }

        LOG(info, "--------------------------------");
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
    return std::make_shared<Win32Gamepad>(
        handle, deviceNameFromHandle(handle));
}
