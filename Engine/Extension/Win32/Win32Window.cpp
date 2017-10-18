#include <Usagi/Engine/Utility/String.hpp>
#include <Usagi/Engine/Runtime/HID/Mouse/MouseEventListener.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/KeyCode.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/KeyEventListener.hpp>

// include dirty windows headers at last
#include "Win32Window.hpp"
#include <ShellScalingAPI.h>

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
    wcex.style = CS_OWNDC;
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

RECT yuki::Win32Window::_getClientScreenRect() const
{
    // Get the window client area.
    RECT rc;
    GetClientRect(mWindowHandle, &rc);

    POINT pt = { rc.left, rc.top };
    POINT pt2 = { rc.right, rc.bottom };
    ClientToScreen(mWindowHandle, &pt);
    ClientToScreen(mWindowHandle, &pt2);

    rc.left = pt.x;
    rc.top = pt.y;
    rc.right = pt2.x;
    rc.bottom = pt2.y;

    return rc;
}

void yuki::Win32Window::_createWindowHandle(const std::string &title, int width, int height)
{
    auto windowTitleWide = string::toWideVector(title);
    // todo update after resizing
    mWindowSize = { width, height };

    static constexpr DWORD window_style = WS_OVERLAPPEDWINDOW;
    static constexpr DWORD window_style_ex = WS_EX_ACCEPTFILES;

    RECT window_rect = { 0 };
    window_rect.bottom = height;
    window_rect.right = width;
    AdjustWindowRectEx(&window_rect, window_style, FALSE, window_style_ex);

    mWindowHandle = CreateWindowEx(
        window_style_ex,
        mWindowClassName,
        &windowTitleWide[0],
        window_style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        window_rect.right - window_rect.left, // width
        window_rect.bottom - window_rect.top, // height
        nullptr,
        nullptr,
        mProcessInstanceHandle,
        nullptr
    );

    if(!mWindowHandle)
    {
        throw std::runtime_error("mWindowHandle() failed");
    }

    // associate the class instance with the window so they can be identified in WindowProc
    SetWindowLongPtr(mWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
}

void yuki::Win32Window::_registerRawInputDevices() const
{
    RAWINPUTDEVICE Rid[2];

    // adds HID mouse, RIDEV_NOLEGACY is not used because we need the system to process non-client area.
    Rid[0].usUsagePage = 0x01;
    Rid[0].usUsage = 0x02;
    Rid[0].dwFlags = 0;
    Rid[0].hwndTarget = 0;

    // adds HID keyboard, RIDEV_NOLEGACY is not used to allow the system process hotkeys like
    // print screen. note that alt+f4 is not handled if related key messages not passed to
    // DefWindowProc(). generally, RIDEV_NOLEGACY should only be used when having a single fullscreen
    // window.
    Rid[1].usUsagePage = 0x01;
    Rid[1].usUsage = 0x06;
    // interestingly, RIDEV_NOHOTKEYS will prevent the explorer from using the fancy window-choosing
    // popup, and we still receive key events when switching window, so it is not used here.
    Rid[1].dwFlags = 0;
    Rid[1].hwndTarget = 0;

    // note that this registration affects the entire application
    if(RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
    {
        //registration failed. Call GetLastError for the cause of the error
        throw std::runtime_error("RegisterRawInputDevices() failed");
    }
}

yuki::Win32Window::Win32Window(const std::string &title, int width, int height)
{
    _ensureWindowSubsystemInitialized();
    _createWindowHandle(title, width, height);
    _registerRawInputDevices();
    Win32Window::showWindow(true);
    Win32Window::centerCursor();
}

HDC yuki::Win32Window::getDeviceContext() const
{
    return GetDC(mWindowHandle);
}

void yuki::Win32Window::showWindow(bool show)
{
    ShowWindow(mWindowHandle, show ? SW_SHOWNORMAL : SW_HIDE);
}

bool yuki::Win32Window::isWindowActive() const
{
    return mWindowActive;
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

Eigen::Vector2f yuki::Win32Window::getWindowSize() const
{
    return mWindowSize.cast<float>();
}

bool yuki::Win32Window::isWindowOpen() const
{
    return !mClosed;
}

LRESULT yuki::Win32Window::_windowMessageDispatcher(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    auto window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    // during window creation some misc messages are sent, we just pass them to the system procedure.
    if(!window)
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return window->_handleWindowMessage(hWnd, message, wParam, lParam);
}

void yuki::Win32Window::_sendButtonEvent(yuki::MouseButtonCode button, bool pressed)
{
    auto &prev_pressed = mMouseButtonDown[static_cast<size_t>(button)];

    // if we receive an release event without a prior press event, it means that
    // the user activated our window by clicking. this is only meaningful if we
    // are in normal mode when using GUI. if immersive mode is enabled, such
    // event may cause undesired behavior, such as firing the weapon.
    if(!pressed && !prev_pressed && isImmersiveMode())
        return;

    prev_pressed = pressed;

    MouseButtonEvent e;
    e.mouse = this;
    e.button = button;
    e.pressed = pressed;
    for(auto &&h : mMouseEventListeners)
    {
        h->onMouseButtonStateChange(e);
    }
}

yuki::KeyCode yuki::Win32Window::_translateKeyCodeFromRawInput(const RAWKEYBOARD *keyboard)
{
    switch(keyboard->VKey)
    {
        case VK_BACK: return KeyCode::BACKSPACE;
        case VK_TAB: return KeyCode::TAB;
        case VK_PAUSE: return KeyCode::PAUSE;
        case VK_CAPITAL: return KeyCode::CAPSLOCK;
        case VK_ESCAPE: return KeyCode::ESCAPE;
        case VK_SPACE: return KeyCode::SPACE;
        case VK_PRIOR: return KeyCode::PAGE_UP;
        case VK_NEXT: return KeyCode::PAGE_DOWN;
        case VK_END: return KeyCode::END;
        case VK_HOME: return KeyCode::HOME;
        case VK_LEFT: return KeyCode::LEFT;
        case VK_UP: return KeyCode::UP;
        case VK_RIGHT: return KeyCode::RIGHT;
        case VK_DOWN: return KeyCode::DOWN;
        case VK_SNAPSHOT: return KeyCode::PRINT_SCREEN;
        case VK_INSERT: return KeyCode::INSERT;
        case VK_DELETE: return KeyCode::DELETE;
        case '0': return KeyCode::DIGIT_0;
        case '1': return KeyCode::DIGIT_1;
        case '2': return KeyCode::DIGIT_2;
        case '3': return KeyCode::DIGIT_3;
        case '4': return KeyCode::DIGIT_4;
        case '5': return KeyCode::DIGIT_5;
        case '6': return KeyCode::DIGIT_6;
        case '7': return KeyCode::DIGIT_7;
        case '8': return KeyCode::DIGIT_8;
        case '9': return KeyCode::DIGIT_9;
        case 'A': return KeyCode::A;
        case 'B': return KeyCode::B;
        case 'C': return KeyCode::C;
        case 'D': return KeyCode::D;
        case 'E': return KeyCode::E;
        case 'F': return KeyCode::F;
        case 'G': return KeyCode::G;
        case 'H': return KeyCode::H;
        case 'I': return KeyCode::I;
        case 'J': return KeyCode::J;
        case 'K': return KeyCode::K;
        case 'L': return KeyCode::L;
        case 'M': return KeyCode::M;
        case 'N': return KeyCode::N;
        case 'O': return KeyCode::O;
        case 'P': return KeyCode::P;
        case 'Q': return KeyCode::Q;
        case 'R': return KeyCode::R;
        case 'S': return KeyCode::S;
        case 'T': return KeyCode::T;
        case 'U': return KeyCode::U;
        case 'V': return KeyCode::V;
        case 'W': return KeyCode::W;
        case 'X': return KeyCode::X;
        case 'Y': return KeyCode::Y;
        case 'Z': return KeyCode::Z;
        case VK_LWIN: return KeyCode::LEFT_OS;
        case VK_RWIN: return KeyCode::RIGHT_OS;
        case VK_APPS: return KeyCode::CONTEXT_MENU;
        case VK_NUMPAD0: return KeyCode::NUM_0;
        case VK_NUMPAD1: return KeyCode::NUM_1;
        case VK_NUMPAD2: return KeyCode::NUM_2;
        case VK_NUMPAD3: return KeyCode::NUM_3;
        case VK_NUMPAD4: return KeyCode::NUM_4;
        case VK_NUMPAD5: return KeyCode::NUM_5;
        case VK_NUMPAD6: return KeyCode::NUM_6;
        case VK_NUMPAD7: return KeyCode::NUM_7;
        case VK_NUMPAD8: return KeyCode::NUM_8;
        case VK_NUMPAD9: return KeyCode::NUM_9;
        case VK_MULTIPLY: return KeyCode::NUM_MULTIPLY;
        case VK_ADD: return KeyCode::NUM_ADD;
        case VK_SUBTRACT: return KeyCode::NUM_SUBTRACT;
        case VK_DECIMAL: return KeyCode::NUM_DECIMAL;
        case VK_DIVIDE: return KeyCode::NUM_DIVIDE;
        case VK_F1: return KeyCode::F1;
        case VK_F2: return KeyCode::F2;
        case VK_F3: return KeyCode::F3;
        case VK_F4: return KeyCode::F4;
        case VK_F5: return KeyCode::F5;
        case VK_F6: return KeyCode::F6;
        case VK_F7: return KeyCode::F7;
        case VK_F8: return KeyCode::F8;
        case VK_F9: return KeyCode::F9;
        case VK_F10: return KeyCode::F10;
        case VK_F11: return KeyCode::F11;
        case VK_F12: return KeyCode::F12;
        case VK_NUMLOCK: return KeyCode::NUM_LOCK;
        case VK_SCROLL: return KeyCode::SCROLL_LOCK;
        case VK_OEM_1: return KeyCode::SEMICOLON;
        case VK_OEM_PLUS: return KeyCode::EQUAL;
        case VK_OEM_COMMA: return KeyCode::COMMA;
        case VK_OEM_MINUS: return KeyCode::MINUS;
        case VK_OEM_PERIOD: return KeyCode::PERIOD;
        case VK_OEM_2: return KeyCode::SLASH;
        case VK_OEM_3: return KeyCode::BACKQUOTE;
        case VK_OEM_4: return KeyCode::LEFT_BRACKET;
        case VK_OEM_5: return KeyCode::BACKSLASH;
        case VK_OEM_6: return KeyCode::RIGHT_BRACKET;
        case VK_OEM_7: return KeyCode::QUOTE;
        default: break;
    }
    int e0_prefixed = (keyboard->Flags & RI_KEY_E0) != 0;
    switch(keyboard->VKey)
    {
        case VK_SHIFT:
        {
            return MapVirtualKey(keyboard->MakeCode, MAPVK_VSC_TO_VK_EX) == VK_LSHIFT ? KeyCode::LEFT_SHIFT : KeyCode::RIGHT_SHIFT;
        }
        case VK_CONTROL: return e0_prefixed ? KeyCode::RIGHT_CONTROL : KeyCode::LEFT_CONTROL;
        case VK_MENU: return e0_prefixed ? KeyCode::RIGHT_ALT : KeyCode::LEFT_ALT;
        case VK_RETURN: return e0_prefixed ? KeyCode::NUM_ENTER : KeyCode::ENTER;
        default: return KeyCode::UNKNOWN;
    }
}

void yuki::Win32Window::_sendKeyEvent(yuki::KeyCode key, bool pressed, bool repeated)
{
    KeyEvent e;
    e.keyboard = this;
    e.keyCode = key;
    e.pressed = pressed;
    e.repeated = repeated;
    for(auto &&h : mKeyEventListeners)
    {
        h->onKeyStateChange(e);
    }
}

void yuki::Win32Window::_confineCursorInClientArea() const
{
    if(!mWindowActive) return;

    RECT client_rect = _getClientScreenRect();
    ClipCursor(&client_rect);
}

void yuki::Win32Window::_processMouseInput(const RAWINPUT *raw)
{
    auto &mouse = raw->data.mouse;

    // only receive relative movement. note that the mouse driver typically won't generate
    // mouse input data based on absolute data.
    // see https://stackoverflow.com/questions/14113303/raw-input-device-rawmouse-usage
    if(mouse.usFlags != MOUSE_MOVE_RELATIVE) return;

    // when in GUI mode, only receive events inside the window rect
    // todo since we use raw input, we receive the mouse messages even if the part of window is covered, in which case the user might perform undesired actions.
    if(!isImmersiveMode())
    {
        auto cursor = getMouseCursorWindowPos();
        if(cursor.x() < 0 || cursor.y() < 0 || cursor.x() >= mWindowSize.x() || cursor.y() >= mWindowSize.y())
            return;
    }

    // process mouse movement
    if(mouse.lLastX || mouse.lLastY)
    {
        MousePositionEvent e;
        e.mouse = this;
        e.cursorPosDelta = { mouse.lLastX, mouse.lLastY };
        for(auto &&h : mMouseEventListeners)
        {
            h->onMouseMove(e);
        }
    }
    // proces mouse buttons & scrolling
    if(mouse.usButtonFlags)
    {
        // process mouse buttons
        // note that it is impossible to activate another window while holding a mouse
        // button pressed within the active window, so it is unnecessary to clear
        // button press states when deactive the window. however this does not hold
        // for the keyboard.
#define _MOUSE_BTN_EVENT(button) \
    if(mouse.usButtonFlags & RI_MOUSE_##button##_BUTTON_DOWN) \
        _sendButtonEvent(MouseButtonCode::button, true); \
    else if(mouse.usButtonFlags & RI_MOUSE_##button##_BUTTON_UP) \
        _sendButtonEvent(MouseButtonCode::button, false) \
/**/
        _MOUSE_BTN_EVENT(LEFT);
        _MOUSE_BTN_EVENT(MIDDLE);
        _MOUSE_BTN_EVENT(RIGHT);
        // ignore other buttons
#undef _MOUSE_BTN_EVENT

        // process scrolling
        if(mouse.usButtonFlags & RI_MOUSE_WHEEL)
        {
            MouseWheelEvent e;
            e.mouse = this;
            e.distance = static_cast<short>(mouse.usButtonData);
            for(auto &&h : mMouseEventListeners)
            {
                h->onMouseWheelScroll(e);
            }
        }
    }
}

std::unique_ptr<BYTE[]> yuki::Win32Window::_getRawInputBuffer(LPARAM lParam) const
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
    std::unique_ptr<BYTE[]> lpb(new BYTE[dwSize]);
    if(GetRawInputData(
        reinterpret_cast<HRAWINPUT>(lParam),
        RID_INPUT,
        lpb.get(),
        &dwSize,
        sizeof(RAWINPUTHEADER)
    ) != dwSize)
        throw std::runtime_error("GetRawInputData does not return correct size!");

    return std::move(lpb);
}

void yuki::Win32Window::_recaptureCursor()
{
    if(mMouseCursorCaptured) _captureCursor();
}

void yuki::Win32Window::_processKeyboardInput(RAWINPUT *raw)
{
    auto &kb = raw->data.keyboard;

    auto key = _translateKeyCodeFromRawInput(&kb);
    // ignore keys other than those on 101 keyboard
    if(key == KeyCode::UNKNOWN) return;

    const bool pressed = (kb.Flags & RI_KEY_BREAK) == 0;
    bool repeated = false;

    if(pressed)
    {
        if(mKeyPressed.count(key) != 0)
            repeated = true;
        else
            mKeyPressed.insert(key);
    }
    else
    {
        mKeyPressed.erase(key);
    }

    _sendKeyEvent(key, pressed, repeated);
}

void yuki::Win32Window::_clearKeyPressedStates()
{
    for(auto iter = mKeyPressed.begin(); iter != mKeyPressed.end();)
    {
        _sendKeyEvent(*iter, false, false);
        iter = mKeyPressed.erase(iter);
    }
}

LRESULT yuki::Win32Window::_handleWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        // unbuffered raw input data
        case WM_INPUT:
        {
            std::unique_ptr<BYTE[]> lpb = _getRawInputBuffer(lParam);
            RAWINPUT *raw = reinterpret_cast<RAWINPUT*>(lpb.get());

            switch(raw->header.dwType)
            {
                case RIM_TYPEKEYBOARD:
                {
                    _processKeyboardInput(raw);
                    break;
                }
                case RIM_TYPEMOUSE:
                {
                    _processMouseInput(raw);
                    break;
                }
                default: break;
            }
            break;
        }
        // window management
        case WM_ACTIVATEAPP:
        {
            // window being activated
            if((mWindowActive = (wParam == TRUE)))
            {
                _recaptureCursor();
            }
            // window being deactivated
            else
            {
                _clearKeyPressedStates();
                break;
            }
            break;
        }
        // todo: sent resize/move events
        case WM_SIZE:
        case WM_MOVE:
        {
            _recaptureCursor();
            break;
        }
        // ignore any key events
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        // ignore mouse events
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
            DestroyWindow(mWindowHandle);
            break;
        }
        case WM_DESTROY:
        {
            mClosed = true;
            PostQuitMessage(0);
            break;
        }
        default:
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    return 0;
}

bool yuki::Win32Window::isKeyPressed(KeyCode key)
{
    return mKeyPressed.count(key) != 0;
}

Eigen::Vector2f yuki::Win32Window::getMouseCursorWindowPos()
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(mWindowHandle, &pt);
    return { pt.x, pt.y };
}

void yuki::Win32Window::_captureCursor()
{
    _confineCursorInClientArea();

    mMouseCursorCaptured = true;
}

void yuki::Win32Window::_releaseCursor()
{
    // remove cursor restriction
    ClipCursor(nullptr);
    mMouseCursorCaptured = false;
}

bool yuki::Win32Window::_isCursorCaptured()
{
    return mMouseCursorCaptured;
}

void yuki::Win32Window::centerCursor()
{
    const auto rect = _getClientScreenRect();
    Eigen::Vector2i cursor {
        (rect.left + rect.right) / 2,
        (rect.top + rect.bottom) / 2
    };
    SetCursorPos(cursor.x(), cursor.y());
}

void yuki::Win32Window::showCursor(bool show)
{
    if(mShowMouseCursor == show) return;

    ShowCursor(show);
    mShowMouseCursor = show;
}

bool yuki::Win32Window::isMouseButtonPressed(MouseButtonCode button) const
{
    const auto idx = static_cast<size_t>(button);
    if(idx > sizeof(mMouseButtonDown) / sizeof(bool)) return false;
    return mMouseButtonDown[idx];
}

HWND yuki::Win32Window::getNativeWindowHandle() const
{
    return mWindowHandle;
}

HINSTANCE yuki::Win32Window::getProcessInstanceHandle()
{
    return mProcessInstanceHandle;
}
