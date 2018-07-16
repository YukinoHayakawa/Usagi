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
    for(auto &&h : mMouseEventListeners)
    {
        h->onMouseButtonStateChange(e);
    }
}

void usagi::Win32Mouse::recaptureCursor()
{
    if(mMouseCursorCaptured) captureCursor();
}

void usagi::Win32Mouse::confineCursorInClientArea() const
{
    if(!mWindow->isFocused()) return;

    const auto client_rect = mWindow->getClientScreenRect();
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
        e.cursor_position_delta = { mouse.lLastX, mouse.lLastY };
        for(auto &&h : mMouseEventListeners)
        {
            h->onMouseMove(e);
        }
    }
    // proces mouse buttons & scrolling
    if(mouse.usButtonFlags)
    {
        // process mouse buttons
        // note that it is impossible to activate another window while holding
        // a mouse button pressed within the active window, so it is
        // unnecessary to clear button press states when deactive the window.
        // however this does not hold for the keyboard.
#define MOUSE_BTN_EVENT(button) \
    if(mouse.usButtonFlags & RI_MOUSE_##button##_BUTTON_DOWN) \
        sendButtonEvent(MouseButtonCode::button, true); \
    else if(mouse.usButtonFlags & RI_MOUSE_##button##_BUTTON_UP) \
        sendButtonEvent(MouseButtonCode::button, false) \
/**/
        MOUSE_BTN_EVENT(LEFT);
        MOUSE_BTN_EVENT(MIDDLE);
        MOUSE_BTN_EVENT(RIGHT);
        // ignore other buttons
#undef MOUSE_BTN_EVENT

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

usagi::Win32Mouse::Win32Mouse(Win32Window *window)
    : mWindow { window }
{
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
    const auto rect = mWindow->getClientScreenRect();
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
