#include "Win32Mouse.hpp"

#include <Usagi/Engine/Runtime/HID/Mouse/MouseEventListener.hpp>

#include "Win32Window.hpp"

void usagi::Win32Mouse::sendButtonEvent(MouseButtonCode button, bool pressed)
{
    auto &prev_pressed = mMouseButtonDown[static_cast<std::size_t>(button)];

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
    forEachListener([&](auto h) {
        h->onMouseButtonStateChange(e);
    });
}

void usagi::Win32Mouse::sendWheelEvent(const Vector2f &distance)
{
    MouseWheelEvent e;
    e.mouse = this;
    e.distance = distance;
    forEachListener([&](auto h) {
        h->onMouseWheelScroll(e);
    });
}

void usagi::Win32Mouse::recaptureCursor()
{
    if(mMouseCursorCaptured) captureCursor();
}

void usagi::Win32Mouse::confineCursorInClientArea() const
{
    if(!mWindow->focused()) return;

    const auto client_rect = mWindow->clientScreenRect();
    ClipCursor(&client_rect);
}

void usagi::Win32Mouse::processMouseInput(const RAWINPUT *raw)
{
    auto &mouse = raw->data.mouse;

    // only receive relative movement. note that the mouse driver typically
    // won't generate mouse input data based on absolute data.
    // see https://stackoverflow.com/questions/14113303/raw-input-device-rawmouse-usage
    if(mouse.usFlags != MOUSE_MOVE_RELATIVE) return;

    // when in GUI mode, only receive events inside the window rect
    // todo since we use raw input, we receive the mouse messages even if the
    // part of window is covered, in which case the user might perform
    // undesired actions.
    if(!isImmersiveMode())
    {
        const auto win_size = mWindow->size();
        const auto cursor = getCursorPositionInWindow();
        if(cursor.x() < 0 || cursor.y() < 0 ||
            cursor.x() >= win_size.x() || cursor.y() >= win_size.y())
            return;
    }

    // process mouse movement
    if(mouse.lLastX || mouse.lLastY)
    {
        MousePositionEvent e;
        e.mouse = this;
        e.distance = { mouse.lLastX, mouse.lLastY };
        forEachListener([&](auto h) {
            h->onMouseMove(e);
        });
    }
    // proces mouse buttons & scrolling
    if(mouse.usButtonFlags)
    {
        // process mouse buttons
        // note that it is impossible to activate another window while holding
        // a mouse button pressed within the active window, so it is
        // unnecessary to clear button press states when deactive the window.
        // however this is not the case for the keyboard.
#define MOUSE_BTN_EVENT(native_code, code) \
        if(mouse.usButtonFlags & RI_MOUSE_##native_code##_DOWN) \
            sendButtonEvent(MouseButtonCode::code, true); \
        else if(mouse.usButtonFlags & RI_MOUSE_##native_code##_UP) \
            sendButtonEvent(MouseButtonCode::code, false) \
/**/
        MOUSE_BTN_EVENT(LEFT_BUTTON, LEFT);
        MOUSE_BTN_EVENT(MIDDLE_BUTTON, MIDDLE);
        MOUSE_BTN_EVENT(RIGHT_BUTTON, RIGHT);
        MOUSE_BTN_EVENT(BUTTON_4, BUTTON_4);
        MOUSE_BTN_EVENT(BUTTON_5, BUTTON_5);
#undef MOUSE_BTN_EVENT

        // process scrolling
        if(mouse.usButtonFlags & RI_MOUSE_WHEEL)
            sendWheelEvent({ 0, static_cast<short>(mouse.usButtonData) });
        // horizontal scrolling, which seems to be undocumented.
        // discovered here: https://stackoverflow.com/questions/7942307/horizontal-mouse-wheel-messages-from-windows-raw-input
        if(mouse.usButtonFlags & RI_MOUSE_HWHEEL)
            sendWheelEvent({ static_cast<short>(mouse.usButtonData), 0 });
    }
}

void usagi::Win32Mouse::onWindowFocusChanged(const WindowFocusEvent &e)
{
    if(e.focused)
    {
        recaptureCursor();
        if(!mShowMouseCursor)
            ShowCursor(false);
    }
    else if(mMouseCursorCaptured) // temporarily release the cursor
    {
        ClipCursor(nullptr);
        if(!mShowMouseCursor)
            ShowCursor(true);
    }
}

void usagi::Win32Mouse::onWindowMoveEnd(const WindowPositionEvent &e)
{
    recaptureCursor();
}

void usagi::Win32Mouse::onWindowResizeEnd(const WindowSizeEvent &e)
{
    recaptureCursor();
}

usagi::Win32Mouse::Win32Mouse(Win32Window *window)
    : mWindow { window }
{
    mWindow->addEventListener(this);
}

usagi::Win32Mouse::~Win32Mouse()
{
    mWindow->removeEventListener(this);
}

usagi::Vector2f usagi::Win32Mouse::getCursorPositionInWindow()
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(mWindow->handle(), &pt);
    return { pt.x, pt.y };
}

void usagi::Win32Mouse::centerCursor()
{
    const auto rect = mWindow->clientScreenRect();
    Vector2i cursor{
        (rect.left + rect.right) / 2,
        (rect.top + rect.bottom) / 2
    };
    SetCursorPos(cursor.x(), cursor.y());
}

void usagi::Win32Mouse::showCursor(bool show)
{
    if(mShowMouseCursor == show) return;

    ShowCursor(show);
    mShowMouseCursor = show;
}

bool usagi::Win32Mouse::isButtonPressed(MouseButtonCode button) const
{
    const auto idx = static_cast<std::size_t>(button);
    if(idx > sizeof(mMouseButtonDown) / sizeof(bool)) return false;
    return mMouseButtonDown[idx];
}

void usagi::Win32Mouse::captureCursor()
{
    confineCursorInClientArea();
    mMouseCursorCaptured = true;
}

void usagi::Win32Mouse::releaseCursor()
{
    // remove cursor restriction
    ClipCursor(nullptr);
    mMouseCursorCaptured = false;
}

bool usagi::Win32Mouse::isCursorCaptured()
{
    return mMouseCursorCaptured;
}
