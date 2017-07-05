#include <Usagi/Engine/Utility/String.hpp>

#include "Window.hpp"

const wchar_t yuki::Window::mWindowClassName[] = L"UsagiNativeWindowWrapper";
HINSTANCE yuki::Window::mProcessInstanceHandle = nullptr;

void yuki::Window::_ensureWindowSubsystemInitialized()
{
    if(mProcessInstanceHandle)
        return;

    // get the process handle, all windows created using this class get dispatched
    // to the same place.
    mProcessInstanceHandle = GetModuleHandle(nullptr);

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    // CS_OWNDC is required to create OpenGL context
    wcex.style = CS_OWNDC | CS_DBLCLKS
    wcex.lpfnWndProc = &_windowMessageDispatcher;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = mProcessInstanceHandle;
    wcex.hIcon = nullptr;
    // hInstance must be null to use predefined cursors
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    // we print the background on our own
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = mWindowClassName;
    wcex.hIconSm = nullptr;

    if(!RegisterClassEx(&wcex))
    {
        throw WindowSubsystemInitializationFailureException("Call to RegisterClassEx failed!");
    }
}

yuki::Window::~Window()
{
}

yuki::Window::Window(const std::string &title, int width, int height)
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
        throw WindowCreationFailureException("Call to CreateWindow failed!");
    }

    // associate the class instance with the window so they can be identified in WindowProc
    SetWindowLongPtr(mWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

void yuki::Window::show()
{
    ShowWindow(mWindowHandle, SW_SHOWNORMAL);
}

void yuki::Window::hide()
{
    ShowWindow(mWindowHandle, SW_HIDE);
}
