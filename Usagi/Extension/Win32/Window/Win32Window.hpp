#pragma once

#include <Usagi/Runtime/Window/Window.hpp>

#include "../Win32.hpp"

namespace usagi
{
enum class MouseButtonCode;

class Win32Window : public Window
{
    friend class Win32WindowManager;

    std::string mTitle;
    HWND mHandle = nullptr;
    Vector2i mPosition;
    Vector2u32 mSize;
    bool mFocused = false;
    bool mClosed = false;
    bool mResizing = false;
    bool mMoving = false;
    wchar_t mHighSurrogate = L'\0';

    static constexpr DWORD WINDOW_STYLE = WS_OVERLAPPEDWINDOW;
    static constexpr DWORD WINDOW_STYLE_EX = WS_EX_ACCEPTFILES;

    /**
     * \brief Right and bottom are used as width and height for the convenience
     * of setting the window position.
     * \return
     */
    RECT getWindowRect() const;
    void updateWindowPosition(HWND window_insert_after = nullptr) const;

    LRESULT handleWindowMessage(
        HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void sendResizeEvent();
    void sendMoveEvent();

public:
    /**
     * \brief Create an empty window.
     * \param manager
     * \param title
     * \param position
     * \param size
     */
    Win32Window(
        Win32WindowManager *manager,
        std::string title,
        Vector2i position,
        Vector2u32 size);
    ~Win32Window() override;

    Vector2i position() const override;
    void setPosition(const Vector2i &position) override;
    void centerWindow() override;
    Vector2u32 size() const override;
    void setSize(const Vector2u32 &size) override;
    void setBorderlessFullscreen() override;

    std::string title() const override;
    void setTitle(std::string title) override;

    void show(bool show) override;
    bool focused() const override;

    bool isOpen() const override;
    void close() override;

    void setResizingEnabled(bool enabled) override;

    Vector2f dpiScale() const override;

    std::string getClipboardText() override;
    void setClipboardText(const std::string &text) override;

    // Win32 helpers

    HDC deviceContext() const;
    HWND handle() const;
    RECT clientScreenRect() const;
};
}
