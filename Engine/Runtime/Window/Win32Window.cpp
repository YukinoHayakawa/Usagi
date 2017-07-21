#include <ShellScalingAPI.h>

#include <Usagi/Engine/Utility/String.hpp>

#include "Win32Window.hpp"

const wchar_t yuki::Win32Window::mWindowClassName[] = L"UsagiNativeWindowWrapper";
HINSTANCE yuki::Win32Window::mProcessInstanceHandle = nullptr;

void yuki::Win32Window::_ensureWindowSubsystemInitialized()
{
    if(mProcessInstanceHandle)
        return;

    // get the process handle, all windows created using this class get dispatched
    // to the same place.
    mProcessInstanceHandle = GetModuleHandle(nullptr);

    WNDCLASSEX wcex = { 0 };

    wcex.cbSize = sizeof(WNDCLASSEX);
    // CS_OWNDC is required to create OpenGL context
    wcex.style = CS_OWNDC | CS_DBLCLKS;
    wcex.lpfnWndProc = &_windowMessageDispatcher;
    wcex.hInstance = mProcessInstanceHandle;
    // hInstance must be null to use predefined cursors
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    // we print the background on our own
    wcex.hbrBackground = nullptr;
    wcex.lpszClassName = mWindowClassName;

    if(!RegisterClassEx(&wcex))
    {
        throw std::runtime_error("RegisterClassEx() failed!");
    }
}

yuki::Win32Window::Win32Window(const std::string &title, int width, int height)
{
    _ensureWindowSubsystemInitialized();

    auto windowTitleWide = string::toWideVector(title);

    mWindowHandle = CreateWindowEx(
        WS_EX_ACCEPTFILES,
        mWindowClassName,
        &windowTitleWide[0],
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        mProcessInstanceHandle,
        nullptr
    );

    if(!mWindowHandle)
    {
        throw std::runtime_error("RegisterClassEx() failed");
    }

    // associate the class instance with the window so they can be identified in WindowProc
    SetWindowLongPtr(mWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
}

HDC yuki::Win32Window::getDeviceContext() const
{
    return GetDC(mWindowHandle);
}

void yuki::Win32Window::show()
{
    ShowWindow(mWindowHandle, SW_SHOWNORMAL);
}

void yuki::Win32Window::hide()
{
    ShowWindow(mWindowHandle, SW_HIDE);
}

void yuki::Win32Window::processEvents()
{
    MSG msg;
    // hwnd should be nullptr or the loop won't end when close the window
    while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT yuki::Win32Window::_windowMessageDispatcher(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
