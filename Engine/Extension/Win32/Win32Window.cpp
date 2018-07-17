#include <Usagi/Engine/Utility/String.hpp>

// include dirty windows headers at last
#include "Win32Window.hpp"
#include <ShellScalingAPI.h>
#pragma comment(lib, "Shcore.lib")

const wchar_t usagi::Win32Window::WINDOW_CLASS_NAME[] =
    L"UsagiWin32WindowWrapper";
HINSTANCE usagi::Win32Window::mProcessInstanceHandle = nullptr;

void usagi::Win32Window::ensureWindowSubsystemInitialized()
{
    if(mProcessInstanceHandle)
        return;

    // get the process handle, all windows created using this class get
    // dispatched to the same place.
    mProcessInstanceHandle = GetModuleHandle(nullptr);

    WNDCLASSEX wcex { };

    wcex.cbSize = sizeof(WNDCLASSEX);
    // CS_OWNDC is required to create OpenGL context
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = &windowMessageDispatcher;
    wcex.hInstance = mProcessInstanceHandle;
    // hInstance must be null to use predefined cursors
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    // we print the background on our own
    wcex.hbrBackground = nullptr;
    wcex.lpszClassName = WINDOW_CLASS_NAME;

    if(!RegisterClassEx(&wcex))
    {
        throw std::runtime_error("RegisterClassEx() failed!");
    }

    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
}

RECT usagi::Win32Window::clientScreenRect() const
{
    // Get the window client area.
    RECT rc;
    GetClientRect(mHandle, &rc);

    POINT pt = { rc.left, rc.top };
    POINT pt2 = { rc.right, rc.bottom };
    ClientToScreen(mHandle, &pt);
    ClientToScreen(mHandle, &pt2);

    rc.left = pt.x;
    rc.top = pt.y;
    rc.right = pt2.x;
    rc.bottom = pt2.y;

    return rc;
}

RECT usagi::Win32Window::getWindowRect() const
{
    RECT window_rect;
    window_rect.left = mPosition.x();
    window_rect.top = mPosition.y();
    window_rect.right = window_rect.left + mSize.x();
    window_rect.bottom = window_rect.top + mSize.y();
    AdjustWindowRectEx(&window_rect, WINDOW_STYLE, FALSE, WINDOW_STYLE_EX);
    window_rect.right -= window_rect.left;
    window_rect.bottom -= window_rect.top;
    return window_rect;
}

void usagi::Win32Window::createWindowHandle(
    const std::string &title,
    const Vector2i &position,
    const Vector2u32 &size)
{
    auto window_title_wide = string::toWideVector(title);

    const auto window_rect = getWindowRect();

    mHandle = CreateWindowEx(
        WINDOW_STYLE_EX,
        WINDOW_CLASS_NAME,
        &window_title_wide[0],
        WINDOW_STYLE,
        window_rect.left, window_rect.top,
        window_rect.right, window_rect.bottom,
        nullptr,
        nullptr,
        mProcessInstanceHandle,
        nullptr
    );

    if(!mHandle)
    {
        throw std::runtime_error("mHandle() failed");
    }

    // associate the class instance with the window so they can be identified 
    // in WindowProc
    SetWindowLongPtr(mHandle, GWLP_USERDATA,
        reinterpret_cast<LONG_PTR>(this));
}

void usagi::Win32Window::registerRawInputDevices() const
{
    RAWINPUTDEVICE Rid[2];

    // adds HID mouse, RIDEV_NOLEGACY is not used because we need the system
    // to process non-client area.
    Rid[0].usUsagePage = 0x01;
    Rid[0].usUsage = 0x02;
    // receives device add/remove messages
    Rid[0].dwFlags = RIDEV_DEVNOTIFY;
    Rid[0].hwndTarget = mHandle;

    // adds HID keyboard, RIDEV_NOLEGACY is not used to allow the system
    // process hotkeys like print screen. note that alt+f4 is not handled
    // if related key messages not passed to DefWindowProc(). generally,
    // RIDEV_NOLEGACY should only be used when having a single fullscreen
    // window.
    Rid[1].usUsagePage = 0x01;
    Rid[1].usUsage = 0x06;
    // interestingly, RIDEV_NOHOTKEYS will prevent the explorer from using
    // the fancy window-choosing popup, and we still receive key events when
    // switching window, so it is not used here.
    Rid[1].dwFlags = RIDEV_DEVNOTIFY;
    Rid[1].hwndTarget = mHandle;

    // note that this registration affects the entire application
    if(RegisterRawInputDevices(Rid, 2, sizeof(RAWINPUTDEVICE)) == FALSE)
    {
        //registration failed. Call GetLastError for the cause of the error
        throw std::runtime_error("RegisterRawInputDevices() failed");
    }
}

usagi::Win32Window::Win32Window(
    const std::string &title,
    const Vector2i &position,
    const Vector2u32 &size)
    : mPosition { position }
    , mSize { size }
    , mKeyboard { this }
    , mMouse { this }
{
    ensureWindowSubsystemInitialized();
    createWindowHandle(title, position, size);
    registerRawInputDevices();
    Win32Window::show(true);
}

HDC usagi::Win32Window::deviceContext() const
{
    return GetDC(mHandle);
}

void usagi::Win32Window::show(bool show)
{
    ShowWindow(mHandle, show ? SW_SHOWNORMAL : SW_HIDE);
}

bool usagi::Win32Window::focused() const
{
    return mFocused;
}

void usagi::Win32Window::processEvents()
{
    MSG msg;
    // hwnd should be nullptr or the loop won't end when close the window
    while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

usagi::Vector2u32 usagi::Win32Window::size() const
{
    return mSize;
}

void usagi::Win32Window::updateWindowPosition() const
{
    const auto window_rect = getWindowRect();
    SetWindowPos(mHandle, nullptr,
        window_rect.left, window_rect.top, window_rect.right,
        window_rect.bottom,
        0
    );
}

void usagi::Win32Window::setSize(const Vector2u32 &size)
{
    mSize = size;
    updateWindowPosition();
    WindowSizeEvent e;
    e.window = this;
    e.size = size;
    forEachListener([&](auto h) {
        h->onWindowResizeBegin(e);
        h->onWindowResized(e);
        h->onWindowResizeEnd(e);
    });
}

usagi::Vector2i usagi::Win32Window::position() const
{
    return mPosition;
}

void usagi::Win32Window::setPosition(const Vector2i &position)
{
    mPosition = position;
    updateWindowPosition();
    WindowPositionEvent e;
    e.window = this;
    e.position = position;
    forEachListener([&](auto h) {
        h->onWindowMoveBegin(e);
        h->onWindowMoved(e);
        h->onWindowMoveEnd(e);
    });
}

bool usagi::Win32Window::isOpen() const
{
    return !mClosed;
}

void usagi::Win32Window::close()
{
    DestroyWindow(mHandle);
}

void usagi::Win32Window::setTitle(const std::string &title)
{
    std::wstring wtitle { title.begin(), title.end() };
    SetWindowText(mHandle, wtitle.c_str());
}

LRESULT usagi::Win32Window::windowMessageDispatcher(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    auto window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hWnd,
        GWLP_USERDATA));
    // during window creation some misc messages are sent, we just pass them
    // to the system procedure.
    if(!window)
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return window->handleWindowMessage(hWnd, message, wParam, lParam);
}

void usagi::Win32Window::fillRawInputBuffer(LPARAM lParam)
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

LRESULT usagi::Win32Window::handleWindowMessage(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        // unbuffered raw input data
        case WM_INPUT:
        {
            fillRawInputBuffer(lParam);
            const auto raw = reinterpret_cast<RAWINPUT*>(
                mRawInputBuffer.data());

            switch(raw->header.dwType)
            {
                case RIM_TYPEKEYBOARD:
                {
                    mKeyboard.processKeyboardInput(raw);
                    break;
                }
                case RIM_TYPEMOUSE:
                {
                    mMouse.processMouseInput(raw);
                    break;
                }
                default: break;
            }
            break;
        }
        // window management
        case WM_ACTIVATEAPP:
        {
            WindowFocusEvent e;
            e.window = this;
            e.focused = mFocused = (wParam == TRUE);
            forEachListener([&](auto h) {
                h->onWindowFocusChanged(e);
            });
            break;
        }
        // resizing & moving
        case WM_ENTERSIZEMOVE:
        {
            WindowSizeEvent e;
            e.window = this;
            e.size = size();
            WindowPositionEvent e2;
            e2.window = this;
            e2.position = position();
            forEachListener([&](auto h) {
                h->onWindowResizeBegin(e);
                h->onWindowMoveBegin(e2);
            });
            break;
        }
        case WM_EXITSIZEMOVE:
        {
            WindowSizeEvent e;
            e.window = this;
            e.size = size();
            WindowPositionEvent e2;
            e2.window = this;
            e2.position = position();
            forEachListener([&](auto h) {
                h->onWindowResizeEnd(e);
                h->onWindowMoveEnd(e2);
            });
            break;
        }
        case WM_SIZE:
        {
            WindowSizeEvent e;
            e.window = this;
            mSize = e.size = { LOWORD(lParam), HIWORD(lParam) };
            forEachListener([&](auto h) {
                h->onWindowResized(e);
            });
            break;
        }
        case WM_MOVE:
        {
            WindowPositionEvent e;
            e.window = this;
            // note that window position is signed
            auto x = static_cast<short>(LOWORD(lParam));
            auto y = static_cast<short>(HIWORD(lParam));
            mPosition = e.position = { x, y };
            forEachListener([&](auto h) {
                h->onWindowMoved(e);
            });
            break;
        }
        // these legacy messages are preserved to make non-client area work
        // properly.
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_MOUSEMOVE:
        case WM_LBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_XBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        {
            break;
        }
        case WM_CHAR:
        {
            break;
        }
        case WM_CLOSE:
        {
            DestroyWindow(mHandle);
            break;
        }
        case WM_DESTROY:
        {
            mClosed = true;
            // todo: support multiple windows
            // PostQuitMessage(0);
            break;
        }
        default:
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    return 0;
}

HWND usagi::Win32Window::handle() const
{
    return mHandle;
}

HINSTANCE usagi::Win32Window::processInstanceHandle()
{
    return mProcessInstanceHandle;
}
