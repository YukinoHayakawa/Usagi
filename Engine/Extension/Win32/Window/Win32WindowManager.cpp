#include "../Win32.hpp"
#include <ShellScalingAPI.h>
#pragma comment(lib, "Shcore.lib")
// some Windows macros are undefined by our header
#include "../Win32MacroFix.hpp"

#include "Win32WindowManager.hpp"

#include <Usagi/Engine/Core/Logging.hpp>

#include "Win32Window.hpp"
#include "../Win32Helper.hpp"

void usagi::Win32WindowManager::registerWindowClass()
{
    // get the process handle, all windows created using this class will have
    // their messages dispatched to our handler
    mProcessInstanceHandle = GetModuleHandleW(nullptr);

    WNDCLASSEXW wcex { };
    wcex.cbSize = sizeof(WNDCLASSEXW);
    // CS_OWNDC is required to create OpenGL context
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = &windowMessageDispatcher;
    wcex.hInstance = mProcessInstanceHandle;
    // hInstance param must be null to use predefined cursors
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    // we print the background using graphics API like Vulkan/OpenGL/DirectX
    wcex.hbrBackground = nullptr;
    wcex.lpszClassName = WINDOW_CLASS_NAME;

    if(!RegisterClassExW(&wcex))
    {
        throw win32::Win32Exception("RegisterClassEx() failed!");
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void usagi::Win32WindowManager::unregisterWindowClass()
{
    UnregisterClassW(WINDOW_CLASS_NAME, mProcessInstanceHandle);
}

usagi::Win32Window * usagi::Win32WindowManager::windowFromHandle(HWND handle)
{
    // make sure the windows is created by our class
    if(GetWindowLongPtrW(handle, GWLP_WNDPROC) != reinterpret_cast<LONG_PTR>(
        &Win32WindowManager::windowMessageDispatcher))
        return nullptr;

    return reinterpret_cast<Win32Window*>(
        GetWindowLongPtrW(handle, GWLP_USERDATA));
}

usagi::Win32WindowManager::Win32WindowManager()
{
    LOG(info, "Creating Win32WindowManager");

    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    registerWindowClass();
}

usagi::Win32WindowManager::~Win32WindowManager()
{
    unregisterWindowClass();
}

LRESULT usagi::Win32WindowManager::windowMessageDispatcher(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(const auto window = windowFromHandle(hWnd))
    {
        return window->handleWindowMessage(hWnd, message, wParam, lParam);
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

void usagi::Win32WindowManager::processMessageRange(UINT min, UINT max)
{
    MSG msg;
    // hWnd should be nullptr in order to clear the queue.
    while(PeekMessageW(&msg, nullptr, min, max, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

usagi::Win32Window * usagi::Win32WindowManager::activeWindow()
{
    // some interesting posts:
    // Eventually, nothing is special any more
    // https://blogs.msdn.microsoft.com/oldnewthing/20081006-00/?p=20643/
    const auto hwnd = GetActiveWindow();

    return windowFromHandle(hwnd);
}

std::shared_ptr<usagi::Window> usagi::Win32WindowManager::createWindow(
    const std::string &title,
    const Vector2i &position,
    const Vector2u32 &size)
{
    return std::make_shared<Win32Window>(this, title, position, size);
}

void usagi::Win32WindowManager::processEvents()
{
    // Skip raw input messages which are processed by Win32InputManager.
    processMessageRange(WM_NULL, WM_INPUT_DEVICE_CHANGE - 1);
    processMessageRange(WM_INPUT + 1, -1);
}
