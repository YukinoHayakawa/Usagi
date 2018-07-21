#include <ShellScalingAPI.h>
#pragma comment(lib, "Shcore.lib")
#include <SetupAPI.h>
#pragma comment(lib, "SetupAPI.lib")

// some Windows macros are undefined by our header
#include "Win32Platform.hpp"

#include <Usagi/Engine/Core/Logging.hpp>

#include "Win32Window.hpp"
#include "Win32Mouse.hpp"
#include "Win32Keyboard.hpp"
#include "Win32Helper.hpp"
#include <Usagi/Engine/Utility/String.hpp>

const wchar_t usagi::Win32Platform::WINDOW_CLASS_NAME[] = L"UsagiRenderWindow";
usagi::Win32Platform *usagi::Win32Platform::mInstance = nullptr;
std::map<std::wstring, std::string> usagi::Win32Platform::mDeviceNames;

void usagi::Win32Platform::registerWindowClass()
{
    // get the process handle, all windows created using this class will have
    // their messages dispatched to our handler
    mProcessInstanceHandle = GetModuleHandle(nullptr);

    WNDCLASSEX wcex { sizeof(WNDCLASSEX) };
    // CS_OWNDC is required to create OpenGL context
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = &windowMessageDispatcher;
    wcex.hInstance = mProcessInstanceHandle;
    // hInstance param must be null to use predefined cursors
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    // we print the background using graphics API like Vulkan/OpenGL/DirectX
    wcex.hbrBackground = nullptr;
    wcex.lpszClassName = WINDOW_CLASS_NAME;

    if(!RegisterClassEx(&wcex))
    {
        throw std::runtime_error("RegisterClassEx() failed!");
    }
}

void usagi::Win32Platform::unregisterWindowClass()
{
    UnregisterClass(WINDOW_CLASS_NAME, mProcessInstanceHandle);
}

void usagi::Win32Platform::registerRawInputDevices()
{
    std::vector<RAWINPUTDEVICE> devices { 3 };

    // For HID APIs, see:
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/hid/
    // For usage page and usage codes, see:
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/hid/top-level-collections-opened-by-windows-for-system-use

    // adds HID mice, RIDEV_NOLEGACY is not used because we need the system
    // to process non-client area.
    devices[0].usUsagePage = 0x01;
    devices[0].usUsage = 0x02;
    // receives device add/remove messages (WM_INPUT_DEVICE_CHANGE)
    devices[0].dwFlags = RIDEV_DEVNOTIFY;
    // receives events from the window with keyboard focus
    devices[0].hwndTarget = nullptr;

    // adds HID keyboards, RIDEV_NOLEGACY is not used to allow the system
    // process hotkeys like print screen. note that alt+f4 is not handled
    // if related key messages not passed to DefWindowProc(). looks like
    // RIDEV_NOLEGACY should only be used when having a single fullscreen
    // window.
    devices[1].usUsagePage = 0x01;
    devices[1].usUsage = 0x06;
    // interestingly, RIDEV_NOHOTKEYS will prevent the explorer from using
    // the fancy window-choosing popup, and we still receive key events when
    // switching window, so it is not used here.
    devices[1].dwFlags = RIDEV_DEVNOTIFY;
    devices[1].hwndTarget = nullptr;

    // adds gamepads
    devices[2].usUsagePage = 0x01;
    devices[2].usUsage = 0x05;
    devices[2].dwFlags = RIDEV_DEVNOTIFY;
    devices[2].hwndTarget = nullptr;

    // note that this registration affects the entire application
    if(RegisterRawInputDevices(
        devices.data(), static_cast<UINT>(devices.size()),
        sizeof(RAWINPUTDEVICE)) == FALSE)
    {
        //registration failed. Call GetLastError for the cause of the error
        throw std::runtime_error("RegisterRawInputDevices() failed");
    }
}

usagi::Win32Window * usagi::Win32Platform::windowFromHandle(HWND handle)
{
    // make sure the windows is created by our class
    if(GetWindowLongPtr(handle, GWLP_WNDPROC) != 
        reinterpret_cast<LONG_PTR>(&Win32Platform::windowMessageDispatcher))
        return nullptr;

    return reinterpret_cast<Win32Window*>(
        GetWindowLongPtr(handle, GWLP_USERDATA));
}

usagi::Win32Window * usagi::Win32Platform::getActiveWindow()
{
    // some interesting posts:
    // Eventually, nothing is special any more
    // https://blogs.msdn.microsoft.com/oldnewthing/20081006-00/?p=20643/
    const auto hwnd = GetActiveWindow();

    return windowFromHandle(hwnd);
}

void usagi::Win32Platform::createVirtualDevices()
{
    mVirtualKeyboard = std::make_shared<Win32Keyboard>(
        nullptr, "Virtual Keyboard"
    );
    mVirtualMouse = std::make_shared<Win32Mouse>(
        nullptr, "Virtual Mouse"
    );
}

LRESULT usagi::Win32Platform::windowMessageDispatcher(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return mInstance->handleWindowMessage(hWnd, message, wParam, lParam);
}

LRESULT usagi::Win32Platform::handleWindowMessage(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    const auto window = windowFromHandle(hWnd);
    auto remove_listeners = false;

    switch(message)
    {
        // unbuffered raw input data
        case WM_INPUT:
        {
            fillRawInputBuffer(lParam);
            const auto raw = reinterpret_cast<RAWINPUT*>(
                mRawInputBuffer.data());

            // dispatch the message to virtual devices
            switch(raw->header.dwType)
            {
                case RIM_TYPEKEYBOARD:
                    mVirtualKeyboard->handleRawInput(raw);
                    break;

                case RIM_TYPEMOUSE:
                    mVirtualMouse->handleRawInput(raw);
                    break;

                case RIM_TYPEHID:
                {
                    static std::vector<CHAR> last;
                    std::vector<CHAR> state {
                        raw->data.hid.bRawData,
                        raw->data.hid.bRawData + raw->data.hid.dwSizeHid };

                    if(state != last)
                    {
                        if(raw->data.hid.dwSizeHid > 80) break;

                        for(DWORD i = 0; i < raw->data.hid.dwSizeHid; i++)
                        {
                            printf("%02X ", raw->data.hid.bRawData[i]);
                        }
                        printf("\n");
                        fflush(stdout);
                    }
                    last = state;
                    break;
                }
                default:
                    break;
            }

            // dispatch the message to specific device
            const auto iter = mRawInputDevices.find(raw->header.hDevice);
            if(iter != mRawInputDevices.end())
            {
                if(const auto dev = iter->second.lock())
                {
                    dev->handleRawInput(raw);
                }
                else // the devices is not used anymore
                {
                    mRawInputDevices.erase(iter);
                }
            }
            return 0;
        }
        // todo: handles device addition/removal
        case WM_INPUT_DEVICE_CHANGE:
        {
            
            break;
        }
        // always hook the virtual keyboard and mouse to the active window.
        case WM_ACTIVATE:
        {
            const auto active = LOWORD(wParam) != WA_INACTIVE;
            if(window)
            {
                if(active)
                {
                    window->addEventListener(mVirtualMouse.get());
                    window->addEventListener(mVirtualKeyboard.get());                    
                }
                else
                {
                    remove_listeners = true;
                }
            }
            // the message should also be process by the window.
            [[fallthrough]];
        }
        // window messages
        default:
        {
            if(window)
            {
                const auto result = window->handleWindowMessage(
                    hWnd, message, wParam, lParam);
                // remove the listners after they were called by the window
                if(remove_listeners)
                {
                    window->removeEventListener(mVirtualMouse.get());
                    window->removeEventListener(mVirtualKeyboard.get());
                }
                return result;
            }
        }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void usagi::Win32Platform::fillRawInputBuffer(LPARAM lParam)
{
    UINT dwSize;

    // fetch raw input data
    GetRawInputData(
        reinterpret_cast<HRAWINPUT>(lParam),
        RID_INPUT,
        nullptr,
        &dwSize,
        sizeof(RAWINPUTHEADER)
    );
    mRawInputBuffer.resize(dwSize);
    if(GetRawInputData(
        reinterpret_cast<HRAWINPUT>(lParam),
        RID_INPUT,
        mRawInputBuffer.data(),
        &dwSize,
        sizeof(RAWINPUTHEADER)
    ) != dwSize)
    {
        throw std::runtime_error(
            "GetRawInputData does not return correct size!"
        );
    }
}

usagi::Win32Platform::Win32Platform()
{
    if(mInstance)
        throw std::runtime_error(
            "Only one instance of Win32Platform can be created!");

    mInstance = this;

    registerWindowClass();
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    registerRawInputDevices();
    createVirtualDevices();
}

usagi::Win32Platform::~Win32Platform()
{
    unregisterWindowClass();

    mInstance = nullptr;
}

std::shared_ptr<usagi::Window> usagi::Win32Platform::createWindow(
    const std::string &title,
    const Vector2i &position,
    const Vector2u32 &size)
{
    return std::make_shared<Win32Window>(this, title, position, size);
}

void usagi::Win32Platform::processEvents()
{
    MSG msg;
    // hwnd should be nullptr or the loop won't end when close the window
    while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

std::shared_ptr<usagi::Keyboard> usagi::Win32Platform::virtualKeyboard() const
{
    return mVirtualKeyboard;
}

std::shared_ptr<usagi::Mouse> usagi::Win32Platform::virtualMouse() const
{
    return mVirtualMouse;
}

const std::vector<std::shared_ptr<usagi::Gamepad>> & usagi::Win32Platform::
    gamepads() const
{
    return { };
}

// Some links:
// Querying kernel objects & following symbolic links:
// http://blogs.microsoft.co.il/pavely/2014/02/05/creating-a-winobj-like-tool/
// https://github.com/hfiref0x/WinObjEx64 (also where I get ntos.h)
// About undocumented Nt fuctions:
// http://undocumented.ntinternals.net/

// Source: Using SetupDi to enumerate devices
// http://samscode.blogspot.com/2009/08/setupdi-how-to-enumerate-devices-using.html
namespace
{
std::wstring getDeviceRegistryProperty(
    __in HDEVINFO hDevInfo,
    __in SP_DEVINFO_DATA DeviceInfoData,
    __in DWORD Property
)
{
    DWORD        data_t;
    std::wstring buffer;
    DWORD        buffer_size = 0;

    // Call function with null to begin with, 
    // then use the returned buffer size (doubled)
    // to Alloc the buffer. Keep calling until
    // success or an unknown failure.
    //
    // Double the returned buffersize to correct
    // for underlying legacy CM functions that 
    // return an incorrect buffersize value on 
    // DBCS/MBCS systems.
    while(!SetupDiGetDeviceRegistryProperty(
        hDevInfo,
        &DeviceInfoData,
        Property,
        &data_t,
        reinterpret_cast<PBYTE>(buffer.data()),
        buffer_size,
        &buffer_size))
    {
        if(ERROR_INSUFFICIENT_BUFFER == GetLastError())
        {
            // Double the size to avoid problems on 
            // W2k MBCS systems per KB 888609. 
            buffer.resize(buffer_size * 2 / sizeof(std::wstring::value_type));
        }
        else
        {
            break;
        }
    }
    // remove extra zeros
    const auto trim_pos = buffer.find(L'\0');
    if(trim_pos != std::string::npos)
        buffer.resize(trim_pos);
    return buffer;
}
}

void usagi::Win32Platform::updateDeviceNames()
{
    // Create a HDEVINFO with all present devices.
    const auto dev_info = SetupDiGetClassDevs(
        nullptr, // GUID
        nullptr, // Enumerator (HID?)
        nullptr,
        DIGCF_PRESENT | DIGCF_ALLCLASSES // find all connected devices
    );

    if(dev_info == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("SetupDiGetClassDevs() failed.");
    }

    mDeviceNames.clear();

    // Enumerate through all devices in Set.
    SP_DEVINFO_DATA device_info_data;
    device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);
    
    for(DWORD i = 0; SetupDiEnumDeviceInfo(dev_info, i, &device_info_data); i++)
    {
        DWORD buffersize = 0;

        const auto device_obj = getDeviceRegistryProperty(
            dev_info, device_info_data, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME);

        if(device_obj.empty()) continue;

        auto name = getDeviceRegistryProperty(
            dev_info, device_info_data, SPDRP_FRIENDLYNAME);
        if(name.empty())
            name = getDeviceRegistryProperty(
                dev_info, device_info_data, SPDRP_DEVICEDESC);

        LOG(info, "{:24}: {}", ws2s(device_obj), ws2s(name));
    }

    const auto last_error = GetLastError();
    if(NO_ERROR != last_error && ERROR_NO_MORE_ITEMS != last_error)
    {
        LOG(error, "Error occured while enumerating devices: {}",
            win32::getErrorMessage(last_error)
        );
    }

    // Cleanup
    SetupDiDestroyDeviceInfoList(dev_info);
}
