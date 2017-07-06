#include <Usagi/Engine/Utility/String.hpp>
#include <Usagi/Engine/Runtime/DevicePlatform/Win32Common.hpp>

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
        throw Win32APIException("RegisterClassEx() failed!");
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
        throw Win32APIException("RegisterClassEx() failed");
    }

    // associate the class instance with the window so they can be identified in WindowProc
    SetWindowLongPtr(mWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
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
    while(GetMessage(&msg, mWindowHandle, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT yuki::Win32Window::_windowMessageDispatcher(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    TCHAR greeting[] = _T("Hello, World!");

    switch(message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);

            // Here your application is laid out.  
            // For this introduction, we just print out "Hello, World!"  
            // in the top left corner.  
            TextOut(hdc,
                5, 5,
                greeting, _tcslen(greeting));
            // End application-specific layout section.  

            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
