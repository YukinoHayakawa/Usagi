﻿#include "Win32Window.hpp"

#include <Usagi/Engine/Utility/String.hpp>
#include <Usagi/Engine/Core/Logging.hpp>

#include "../Win32Helper.hpp"
#include "Win32WindowManager.hpp"

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

usagi::Win32Window::Win32Window(
    Win32WindowManager *manager,
    std::string title,
    Vector2i position,
    Vector2u32 size)
    : mTitle { std::move(title) }
    , mPosition { std::move(position) }
    , mSize { std::move(size) }
{
    LOG(info, "Creating window: {}", title);

    auto window_title_wide = s2ws(mTitle);

    const auto window_rect = getWindowRect();

    mHandle = CreateWindowExW(
        WINDOW_STYLE_EX,
        manager->WINDOW_CLASS_NAME,
        window_title_wide.c_str(),
        WINDOW_STYLE,
        window_rect.left, window_rect.top,
        window_rect.right, window_rect.bottom,
        nullptr,
        nullptr,
        manager->processInstanceHandle(),
        nullptr
    );

    if(!mHandle)
    {
        throw win32::Win32Exception("CreateWindowEx() failed");
    }

    // associate the class instance with the window so they can be identified 
    // in WindowProc
    SetWindowLongPtrW(mHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

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

std::string usagi::Win32Window::title() const
{
    // Don't use GetWindowText(), it may cause deadlock.
    return mTitle;
}

void usagi::Win32Window::setTitle(std::string title)
{
    mTitle = std::move(title);
    SetWindowTextW(mHandle, s2ws(mTitle).c_str());
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

LRESULT usagi::Win32Window::handleWindowMessage(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    assert(hWnd == mHandle);

    switch(message)
    {
        // window focus change
        case WM_ACTIVATE:
        {
            const auto active = LOWORD(wParam) != WA_INACTIVE;
            WindowFocusEvent e;
            e.window = this;
            e.focused = mFocused = active;
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
            break;
        }
        default:
        {
            return DefWindowProcW(hWnd, message, wParam, lParam);
        }
    }
    return 0;
}

HWND usagi::Win32Window::handle() const
{
    return mHandle;
}