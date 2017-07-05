#pragma once

#include <string>
#include <vector>
// todo: hide windows headers
#include <windows.h>
#include <tchar.h>

#include "Window.hpp"

namespace yuki
{

class Win32Window : Window
{
private:
    static const wchar_t mWindowClassName[];
    static HINSTANCE mProcessInstanceHandle;

    HWND mWindowHandle = nullptr;
    
    /**
    * \brief Initialize the window subsystem.
    */
    static void _ensureWindowSubsystemInitialized();

public:
    /**
     * \brief Create an empty window.
     * \param title 
     * \param width 
     * \param height 
     */
    Win32Window(const std::string &title, int width, int height);

    HDC getDeviceContext() const;

    void show() override;
    void hide() override;

    //virtual void processEvents();

    void loop();

    static LRESULT CALLBACK _windowMessageDispatcher(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

}
