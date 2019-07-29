#include "../Win32.hpp"

#include "Win32WindowManager.hpp"

#include <Usagi/Core/Logging.hpp>

#include "Win32Window.hpp"
#include "../Win32Helper.hpp"

std::map<HWND, usagi::Win32Window *> usagi::Win32WindowManager::mWindows;

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
    // vulkan swapchain somehow changes the WndProc, so don't use
    // GWLP_WNDPROC to identify the windows
    const auto i = mWindows.find(handle);
    if(i != mWindows.end())
        return i->second;
    return nullptr;
}

usagi::Win32WindowManager::Win32WindowManager()
{
    LOG(info, "Creating Win32WindowManager");

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

usagi::Win32Window * usagi::Win32WindowManager::activeWindow()
{
    // some interesting posts:
    // Eventually, nothing is special any more
    // https://blogs.msdn.microsoft.com/oldnewthing/20081006-00/?p=20643/
    const auto hwnd = GetActiveWindow();

    return windowFromHandle(hwnd);
}

std::shared_ptr<usagi::Window> usagi::Win32WindowManager::createWindow(
    std::string_view title,
    const Vector2i &position,
    const Vector2u32 &size)
{
    return std::make_shared<Win32Window>(
        this, std::string { title }, position, size
    );
}

usagi::Vector2u32 usagi::Win32WindowManager::currentDisplayResolution()
{
    return {
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN)
    };
}

void usagi::Win32WindowManager::processEvents()
{
    // Win32WindowManager and Win32InputManager both has a message loop that
    // processes messages for any windows in the current thread to prevent
    // leaving some messages unprocessed in the queue, which causes Windows
    // to believe our windows are unresponsive. therefore the order of
    // calling processEvents() on these two managers is insignificant on
    // Windows.
    MSG msg;
    while(PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}
