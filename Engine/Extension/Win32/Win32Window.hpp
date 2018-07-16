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

    HWND mWindowHandle = nullptr;
    Vector2u32 mWindowSize;
    bool mWindowActive = false;
    bool mClosed = false;

    static void ensureWindowSubsystemInitialized();

    void createWindowHandle(const std::string &title, int width, int height);
    void registerRawInputDevices() const;

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
     * \param size 
     */
    Win32Window(const std::string &title, const Vector2u32 &size);

    Vector2f size() const override;
    void setTitle(const std::string &title) override;

    void show(bool show) override;
    bool isFocused() const override;
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
    RECT getClientScreenRect() const;
};
}
