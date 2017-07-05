#pragma once

#include <string>
#include <vector>
// todo: hide windows headers
#include <windows.h>
#include <tchar.h>

#include <Usagi/Engine/Utility/Exception.hpp>

namespace yuki
{

YUKI_DECL_RUNTIME_EXCEPTION(WindowSubsystemInitializationFailure);
YUKI_DECL_RUNTIME_EXCEPTION(WindowCreationFailure);

/**
 * \brief Window is the interface between the player and the engine. It is mainly for
 * receive user inputs, but can also be extended to render graphical outputs.
 */
class Window
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
    virtual ~Window();

    /**
     * \brief Create an empty window.
     * \param title 
     * \param width 
     * \param height 
     */
    Window(const std::string &title, int width, int height);

    virtual void show();
    virtual void hide();

    //virtual void processEvents();



    void loop()
    {
        MSG msg;
        while(GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    static LRESULT CALLBACK _windowMessageDispatcher(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        PAINTSTRUCT ps;
        HDC hdc;
        TCHAR greeting[] = _T("Hello, World!");

        switch(message)
        {
            case WM_PAINT:
                hdc = BeginPaint(hWnd, &ps);

                // Here your application is laid out.  
                // For this introduction, we just print out "Hello, World!"  
                // in the top left corner.  
                TextOut(hdc,
                    5, 5,
                    greeting, _tcslen(greeting));
                // End application-specific layout section.  

                EndPaint(hWnd, &ps);
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return 0;
    }
};

}
