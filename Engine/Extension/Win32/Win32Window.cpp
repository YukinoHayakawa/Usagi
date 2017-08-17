#include <Usagi/Engine/Utility/String.hpp>
#include <Usagi/Engine/Runtime/HID/Mouse/MouseEventListener.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/KeyCode.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/KeyEventListener.hpp>

// include dirty windows headers at last
#include "Win32Window.hpp"
#include <ShellScalingAPI.h>
#include <Windowsx.h>
#include <Strsafe.h>

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

    Rid[0].usUsagePage = 0x01;
    Rid[0].usUsage = 0x02;
    Rid[0].dwFlags = 0; // RIDEV_NOLEGACY;   // adds HID mouse and also ignores legacy mouse messages
    Rid[0].hwndTarget = 0;

    Rid[1].usUsagePage = 0x01;
    Rid[1].usUsage = 0x06;
    Rid[1].dwFlags = 0; // RIDEV_NOLEGACY;   // adds HID keyboard and also ignores legacy keyboard messages
    Rid[1].hwndTarget = mWindowHandle;

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
    auto window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    // during window creation some misc messages are sent, we just pass them to the system procedure.
    if(!window)
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return window->_handleWindowMessage(hWnd, message, wParam, lParam);
}

Eigen::Vector2i yuki::Win32Window::_parseMousePos(LPARAM lParam)
{
    auto xPos = GET_X_LPARAM(lParam);
    auto yPos = GET_Y_LPARAM(lParam);
    return { xPos, yPos };
}

void yuki::Win32Window::_sendButtonEvent(yuki::MouseButtonCode button, bool pressed)
{
    MouseButtonEvent e;
    e.mouse = this;
    e.button = button;
    e.pressed = pressed;
    for(auto &&h : mMouseEventListeners)
    {
        h->onMouseButtonStateChange(e);
    }
}

yuki::KeyCode yuki::Win32Window::_translateKeyCodeFromMessage(WPARAM vkey, LPARAM lParam)
{
    switch(vkey)
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
        case VK_SHIFT: case VK_CONTROL: case VK_MENU: case VK_RETURN: break;
        default: return KeyCode::UNKNOWN;
    }
    int extended = (lParam & 0x01000000) != 0;
    switch(vkey)
    {
        case VK_SHIFT:
        {
            UINT scancode = (lParam & 0x00ff0000) >> 16;
            return MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX) == VK_LSHIFT ? KeyCode::LEFT_SHIFT : KeyCode::RIGHT_SHIFT;
        }
        case VK_CONTROL: return extended ? KeyCode::RIGHT_CONTROL : KeyCode::LEFT_CONTROL;
        case VK_MENU: return extended ? KeyCode::RIGHT_ALT : KeyCode::LEFT_ALT;
        case VK_RETURN: return extended ? KeyCode::NUM_ENTER : KeyCode::ENTER;
        default: return KeyCode::UNKNOWN;
    }
}

int yuki::Win32Window::_translateKeyCodeToNative(KeyCode key)
{
    // note that the two enter keys are not included
    switch(key)
    {
        case KeyCode::BACKSPACE: return VK_BACK;
        case KeyCode::TAB: return VK_TAB;
        case KeyCode::PAUSE: return VK_PAUSE;
        case KeyCode::CAPSLOCK: return VK_CAPITAL;
        case KeyCode::ESCAPE: return VK_ESCAPE;
        case KeyCode::SPACE: return VK_SPACE;
        case KeyCode::PAGE_UP: return VK_PRIOR;
        case KeyCode::PAGE_DOWN: return VK_NEXT;
        case KeyCode::END: return VK_END;
        case KeyCode::HOME: return VK_HOME;
        case KeyCode::LEFT: return VK_LEFT;
        case KeyCode::UP: return VK_UP;
        case KeyCode::RIGHT: return VK_RIGHT;
        case KeyCode::DOWN: return VK_DOWN;
        case KeyCode::PRINT_SCREEN: return VK_SNAPSHOT;
        case KeyCode::INSERT: return VK_INSERT;
        case KeyCode::DELETE: return VK_DELETE;
        case KeyCode::DIGIT_0: return '0';
        case KeyCode::DIGIT_1: return '1';
        case KeyCode::DIGIT_2: return '2';
        case KeyCode::DIGIT_3: return '3';
        case KeyCode::DIGIT_4: return '4';
        case KeyCode::DIGIT_5: return '5';
        case KeyCode::DIGIT_6: return '6';
        case KeyCode::DIGIT_7: return '7';
        case KeyCode::DIGIT_8: return '8';
        case KeyCode::DIGIT_9: return '9';
        case KeyCode::A: return 'A';
        case KeyCode::B: return 'B';
        case KeyCode::C: return 'C';
        case KeyCode::D: return 'D';
        case KeyCode::E: return 'E';
        case KeyCode::F: return 'F';
        case KeyCode::G: return 'G';
        case KeyCode::H: return 'H';
        case KeyCode::I: return 'I';
        case KeyCode::J: return 'J';
        case KeyCode::K: return 'K';
        case KeyCode::L: return 'L';
        case KeyCode::M: return 'M';
        case KeyCode::N: return 'N';
        case KeyCode::O: return 'O';
        case KeyCode::P: return 'P';
        case KeyCode::Q: return 'Q';
        case KeyCode::R: return 'R';
        case KeyCode::S: return 'S';
        case KeyCode::T: return 'T';
        case KeyCode::U: return 'U';
        case KeyCode::V: return 'V';
        case KeyCode::W: return 'W';
        case KeyCode::X: return 'X';
        case KeyCode::Y: return 'Y';
        case KeyCode::Z: return 'Z';
        case KeyCode::CONTEXT_MENU: return VK_APPS;
        case KeyCode::NUM_0: return VK_NUMPAD0;
        case KeyCode::NUM_1: return VK_NUMPAD1;
        case KeyCode::NUM_2: return VK_NUMPAD2;
        case KeyCode::NUM_3: return VK_NUMPAD3;
        case KeyCode::NUM_4: return VK_NUMPAD4;
        case KeyCode::NUM_5: return VK_NUMPAD5;
        case KeyCode::NUM_6: return VK_NUMPAD6;
        case KeyCode::NUM_7: return VK_NUMPAD7;
        case KeyCode::NUM_8: return VK_NUMPAD8;
        case KeyCode::NUM_9: return VK_NUMPAD9;
        case KeyCode::NUM_MULTIPLY: return VK_MULTIPLY;
        case KeyCode::NUM_ADD: return VK_ADD;
        case KeyCode::NUM_SUBTRACT: return VK_SUBTRACT;
        case KeyCode::NUM_DECIMAL: return VK_DECIMAL;
        case KeyCode::NUM_DIVIDE: return VK_DIVIDE;
        case KeyCode::F1: return VK_F1;
        case KeyCode::F2: return VK_F2;
        case KeyCode::F3: return VK_F3;
        case KeyCode::F4: return VK_F4;
        case KeyCode::F5: return VK_F5;
        case KeyCode::F6: return VK_F6;
        case KeyCode::F7: return VK_F7;
        case KeyCode::F8: return VK_F8;
        case KeyCode::F9: return VK_F9;
        case KeyCode::F10: return VK_F10;
        case KeyCode::F11: return VK_F11;
        case KeyCode::F12: return VK_F12;
        case KeyCode::NUM_LOCK: return VK_NUMLOCK;
        case KeyCode::SCROLL_LOCK: return VK_SCROLL;
        case KeyCode::SEMICOLON: return VK_OEM_1;
        case KeyCode::EQUAL: return VK_OEM_PLUS;
        case KeyCode::COMMA: return VK_OEM_COMMA;
        case KeyCode::MINUS: return VK_OEM_MINUS;
        case KeyCode::PERIOD: return VK_OEM_PERIOD;
        case KeyCode::SLASH: return VK_OEM_2;
        case KeyCode::BACKQUOTE: return VK_OEM_3;
        case KeyCode::LEFT_BRACKET: return VK_OEM_4;
        case KeyCode::BACKSLASH: return VK_OEM_5;
        case KeyCode::RIGHT_BRACKET: return VK_OEM_6;
        case KeyCode::QUOTE: return VK_OEM_7;
        case KeyCode::LEFT_SHIFT: return VK_LSHIFT;
        case KeyCode::RIGHT_SHIFT: return VK_RSHIFT;
        case KeyCode::LEFT_CONTROL: return VK_LCONTROL;
        case KeyCode::RIGHT_CONTROL: return VK_RCONTROL;
        case KeyCode::LEFT_ALT: return VK_LMENU;
        case KeyCode::RIGHT_ALT: return VK_RMENU;
        case KeyCode::LEFT_OS: return VK_LWIN;
        case KeyCode::RIGHT_OS: return VK_RWIN;
        default: return 0;
    }
}

void yuki::Win32Window::_sendKeyEvent(WPARAM wParam, LPARAM lParam, bool pressed, bool repeated)
{
    KeyEventListener::KeyEvent e;
    e.keyCode = _translateKeyCodeFromMessage(wParam, lParam);
    e.pressed = pressed;
    e.repeated = repeated;
    // store key state for later queries
    switch(e.keyCode)
    {
        case KeyCode::ENTER: mEnterPressed = pressed; break;
        case KeyCode::NUM_ENTER: mNumEnterPressed = pressed; break;
        default: break;
    }
    for(auto &&h : mKeyEventListeners)
    {
        h->onKeyStateChange(e);
    }
}

void yuki::Win32Window::_confineCursorInClientArea() const
{
    RECT client_rect = _getClientScreenRect();
    ClipCursor(&client_rect);
}

void yuki::Win32Window::_processMouseInput(const RAWINPUT *raw)
{
    TCHAR szTempOutput[1024];
    StringCchPrintf(szTempOutput, 1024, TEXT("Mouse: usFlags=%04x ulButtons=%04x usButtonFlags=%04x usButtonData=%04x ulRawButtons=%04x lLastX=%04x lLastY=%04x ulExtraInformation=%04x\r\n"),
        raw->data.mouse.usFlags,
        raw->data.mouse.ulButtons,
        raw->data.mouse.usButtonFlags,
        raw->data.mouse.usButtonData,
        raw->data.mouse.ulRawButtons,
        raw->data.mouse.lLastX,
        raw->data.mouse.lLastY,
        raw->data.mouse.ulExtraInformation);
    OutputDebugString(szTempOutput);

    auto &mouse = raw->data.mouse;

    // only receive relative movement. note that the mouse driver typically won't generate
    // mouse input data based on absolute data.
    // see https://stackoverflow.com/questions/14113303/raw-input-device-rawmouse-usage
    if(mouse.usFlags != MOUSE_MOVE_RELATIVE) return;

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
#define _MOUSE_BTN_EVENT(button) \
    if(mouse.usButtonFlags & RI_MOUSE_##button##_BUTTON_DOWN) \
        _sendButtonEvent(MouseButtonCode::button, true); \
    else if(mouse.usButtonFlags & RI_MOUSE_##button##_BUTTON_UP) \
        _sendButtonEvent(MouseButtonCode::button, false) \
/**/
        // todo send mouse button up events when the window is deactivated
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

LRESULT yuki::Win32Window::_handleWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        // unbuffered raw input data
        case WM_INPUT:
        {
            TCHAR szTempOutput[1024];

            std::unique_ptr<BYTE[]> lpb = _getRawInputBuffer(lParam);
            RAWINPUT *raw = reinterpret_cast<RAWINPUT*>(lpb.get());

            switch(raw->header.dwType)
            {
                case RIM_TYPEKEYBOARD:
                {
                    StringCchPrintf(szTempOutput, 1024, TEXT(" Kbd: make=%04x Flags:%04x Reserved:%04x ExtraInformation:%08x, msg=%04x VK=%04x \n"),
                        raw->data.keyboard.MakeCode,
                        raw->data.keyboard.Flags,
                        raw->data.keyboard.Reserved,
                        raw->data.keyboard.ExtraInformation,
                        raw->data.keyboard.Message,
                        raw->data.keyboard.VKey);
                    OutputDebugString(szTempOutput);
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
        // key strokes
        // todo: print screen receive no press event
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        {
            _sendKeyEvent(wParam, lParam, true, (lParam & (1 << 30)) != 0);
            break;
        }
        case WM_SYSKEYUP:
        case WM_KEYUP:
        {
            _sendKeyEvent(wParam, lParam, false, false);
            break;
        }
        // window management
        case WM_ACTIVATEAPP:
        {
            mWindowActive = wParam == TRUE;
            if(mMouseCursorCaptured) captureCursor();
            break;
        }
        // todo: sent resize/move events
        case WM_SIZE:
        case WM_MOVE:
        {
            if(mMouseCursorCaptured) captureCursor();
            break;
        }
        case WM_DESTROY:
        {
            // todo: fix the message loop does not exit after closing the window
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
    return false;
}

Eigen::Vector2f yuki::Win32Window::getMouseCursorWindowPos()
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(mWindowHandle, &pt);
    return { pt.x, pt.y };
}

void yuki::Win32Window::captureCursor()
{
    _confineCursorInClientArea();

    mMouseCursorCaptured = true;
}

void yuki::Win32Window::releaseCursor()
{
    // remove cursor restriction
    ClipCursor(nullptr);
    mMouseCursorCaptured = false;
}

bool yuki::Win32Window::isCursorCaptured()
{
    return mMouseCursorCaptured;
}

void yuki::Win32Window::centerCursor()
{
    auto rect = _getClientScreenRect();
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
