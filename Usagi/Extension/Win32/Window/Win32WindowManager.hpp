#pragma once

#include <map>

#include <Usagi/Runtime/Window/WindowManager.hpp>
#include <Usagi/Utility/Singleton.hpp>

#include "../Win32.hpp"

namespace usagi
{
class Win32WindowManager
    : Singleton<Win32WindowManager>
    , public WindowManager
{
    friend class Win32Window;

    HINSTANCE mProcessInstanceHandle = nullptr;

    void registerWindowClass();
    void unregisterWindowClass();

    static std::map<HWND, Win32Window *> mWindows;

    /**
     * \brief Get the window pointer from a Win32 window handle. If the window
     * is not created by this class, a nullptr is returned.
     * \return
     */
    static Win32Window * windowFromHandle(HWND handle);

    static LRESULT CALLBACK windowMessageDispatcher(
        HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
    Win32WindowManager();
    ~Win32WindowManager();

    std::shared_ptr<Window> createWindow(
        std::string_view title,
        const Vector2i &position,
        const Vector2u32 &size) override;

    Vector2u32 currentDisplayResolution() override;
    static Vector2u32 getCurrentDisplayResolution();

    void processEvents() override;

    // Win32

    static inline const wchar_t WINDOW_CLASS_NAME[] = L"UsagiNativeWindow";

    HINSTANCE processInstanceHandle() const { return mProcessInstanceHandle; }

    /**
     * \brief Get the window having input focus. If the foreground window
     * is not owned by our process, nullptr is returned.
     * \return
     */
    static Win32Window * activeWindow();
};
}
