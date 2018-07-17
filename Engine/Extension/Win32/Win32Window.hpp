#pragma once

#include <Usagi/Engine/Runtime/Window/Window.hpp>

#include "WindowsHeader.hpp"
#include "Win32Keyboard.hpp"
#include "Win32Mouse.hpp"

namespace usagi
{
enum class MouseButtonCode;

class Win32Window : public Window
{
    static const wchar_t WINDOW_CLASS_NAME[];
    static HINSTANCE mProcessInstanceHandle;

    static void ensureWindowSubsystemInitialized();

    void createWindowHandle(
        const std::string &title,
        const Vector2i &position,
        const Vector2u32 &size);
    void registerRawInputDevices() const;

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

    static LRESULT CALLBACK windowMessageDispatcher(
        HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT handleWindowMessage(
        HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam);

    std::unique_ptr<BYTE[]> getRawInputBuffer(LPARAM lParam) const;

    Win32Keyboard mKeyboard;
    Win32Mouse mMouse;

public:
    /**
     * \brief Create an empty window.
     * \param title
     * \param position 
     * \param size 
     */
    Win32Window(
        const std::string &title,
        const Vector2i &position,
        const Vector2u32 &size);

    Vector2i position() const override;
    void setPosition(const Vector2i &position) override;

    Vector2u32 size() const override;
    void setSize(const Vector2u32 &size) override;

    void setTitle(const std::string &title) override;

    void show(bool show) override;
    bool focused() const override;

    bool isOpen() const override;
    void close() override;

    void processEvents() override;

    // devices

    Mouse * mouse() { return &mMouse; }
    Keyboard * keyboard() { return &mKeyboard; }

    // Win32 helpers

    static HINSTANCE processInstanceHandle();
    HDC deviceContext() const;
    HWND handle() const;
    RECT clientScreenRect() const;
};
}
