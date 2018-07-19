#pragma once

#include <Usagi/Engine/Runtime/Window/Window.hpp>

#include "Win32.hpp"
#include "Win32Keyboard.hpp"
#include "Win32Mouse.hpp"

namespace usagi
{
enum class MouseButtonCode;

class Win32Window : public Window
{
    friend class Win32Platform;

    HWND mHandle = nullptr;
    Vector2i mPosition;
    Vector2u32 mSize;
    bool mFocused = false;
    bool mClosed = false;
    static constexpr DWORD WINDOW_STYLE = WS_OVERLAPPEDWINDOW;
    static constexpr DWORD WINDOW_STYLE_EX = WS_EX_ACCEPTFILES;

    /**
     * \brief Right and bottom are used as width and height for the convenience
     * of setting the window position.
     * \return 
     */
    RECT getWindowRect() const;
    void updateWindowPosition() const;

    LRESULT handleWindowMessage(
        HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam);

public:
    /**
     * \brief Create an empty window.
     * \param platform
     * \param title
     * \param position 
     * \param size 
     */
    Win32Window(
        Win32Platform *platform,
        const std::string &title,
        Vector2i position,
        Vector2u32 size);

    Vector2i position() const override;
    void setPosition(const Vector2i &position) override;

    Vector2u32 size() const override;
    void setSize(const Vector2u32 &size) override;

    void setTitle(const std::string &title) override;

    void show(bool show) override;
    bool focused() const override;

    bool isOpen() const override;
    void close() override;

    // Win32 helpers

    HDC deviceContext() const;
    HWND handle() const;
    RECT clientScreenRect() const;
};
}
