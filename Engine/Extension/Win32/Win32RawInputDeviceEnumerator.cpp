#include "Win32RawInputDeviceEnumerator.hpp"

#include <vector>

#include <loguru.hpp>

void usagi::Win32RawInputDeviceEnumerator::enumerateDevices()
{
    LOG_F(INFO, "Enumerating raw input devices");

    UINT num_devices;
    std::vector<RAWINPUTDEVICELIST> raw_input_device_list;
    if(GetRawInputDeviceList(
        nullptr, &num_devices, sizeof(RAWINPUTDEVICELIST)) != 0)
        throw std::runtime_error("Could not get raw input device amount.");
    raw_input_device_list.resize(num_devices);
    if(GetRawInputDeviceList(raw_input_device_list.data(), &num_devices,
        sizeof(RAWINPUTDEVICELIST)) == static_cast<unsigned>(-1))
        throw std::runtime_error("Could not get raw input device list.");

    for(auto &&device : raw_input_device_list)
    {
        // get device name
        UINT size;
        std::wstring name = L"<Error getting device name>";
        if(GetRawInputDeviceInfo(device.hDevice, RIDI_DEVICENAME,
            nullptr, &size) == 0)
        {
            name.resize(size, L'\0');
            GetRawInputDeviceInfo(device.hDevice, RIDI_DEVICENAME,
                name.data(), &size);
        }

        // get device info
        RID_DEVICE_INFO device_info { sizeof(RID_DEVICE_INFO) };
        size = device_info.cbSize;
        GetRawInputDeviceInfo(device.hDevice, RIDI_DEVICEINFO,
            &device_info, &size);

        // 126 is max length of USB devices
        const std::size_t max_size = 126;
        std::wstring device_name;
        //device_name.resize(max_size, L'\0');
        //auto handle = CreateFile(
        //    name.c_str(),
        //    GENERIC_READ,
        //    FILE_SHARE_READ | FILE_SHARE_WRITE,
        //    nullptr, OPEN_EXISTING, 0, nullptr);
        //LOG_S(INFO)  << GetLastError();
        //if(handle)
        //{
        //    HidD_GetProductString(handle, device_name.data(),
        //        sizeof(wchar_t) * max_size);
        //    CloseHandle(handle);
        //    handle = nullptr;
        //}

        LOG_SCOPE_F(INFO, "");

        LOG_F(INFO,    "Name:                       %ls", name.c_str());
        LOG_F(INFO,    "Friendly Name:              %ls", device_name.c_str());

        switch(device_info.dwType)
        {
            case RIM_TYPEMOUSE:
            {
                LOG_S(INFO) << "Device Type:                "
                    << "Mouse";
                LOG_S(INFO) << "Id:                         "
                    << device_info.mouse.dwId;
                LOG_S(INFO) << "Number of Buttons:          "
                    << device_info.mouse.dwNumberOfButtons;
                LOG_S(INFO) << "Sample rate:                "
                    << device_info.mouse.dwSampleRate;
                break;
            }
            case RIM_TYPEKEYBOARD:
            {
                LOG_S(INFO) << "Device Type:                "
                    << "Keyboard";
                LOG_S(INFO) << "Keyboard Mode:              "
                    << device_info.keyboard.dwKeyboardMode;
                LOG_S(INFO) << "Number of function keys:    "
                    << device_info.keyboard.dwNumberOfFunctionKeys;
                LOG_S(INFO) << "Number of indicators:       "
                    << device_info.keyboard.dwNumberOfIndicators;
                LOG_S(INFO) << "Number of keys total:       "
                    << device_info.keyboard.dwNumberOfKeysTotal;
                LOG_S(INFO) << "Type of the keyboard:       "
                    << device_info.keyboard.dwType;
                LOG_S(INFO) << "Subtype of the keyboard:    "
                    << device_info.keyboard.dwSubType;
                break;
            }
            case RIM_TYPEHID:
            {
                LOG_S(INFO) << "Device Type:                "
                    << "HID";
                LOG_S(INFO) << "Vendor Id:                  "
                    << device_info.hid.dwVendorId;
                LOG_S(INFO) << "Product Id:                 "
                    << device_info.hid.dwProductId;
                LOG_S(INFO) << "Version No:                 "
                    << device_info.hid.dwVersionNumber;
                LOG_S(INFO) << "Usage Page:                 "
                    << device_info.hid.usUsagePage;
                LOG_S(INFO) << "Usage:                      "
                    << device_info.hid.usUsage;
                break;
            }
            default:
            {
                LOG_S(INFO) << "Device Type:                "
                    << "Unknown";
                break;
            }
        }
    }
}
