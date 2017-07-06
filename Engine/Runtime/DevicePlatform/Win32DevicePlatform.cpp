#include <Usagi/Engine/Runtime/Window/Win32Window.hpp>
#include <Usagi/Engine/Runtime/GraphicsInterface/OpenGL/Context/WGLContext.hpp>
#include <Usagi/Engine/Runtime/GraphicsInterface/OpenGL/OpenGLCoreGraphicsInterface.hpp>
#include <Usagi/Engine/Runtime/RenderWindow.hpp>

#include "Win32DevicePlatform.hpp"

std::shared_ptr<yuki::RenderWindow> yuki::Win32DevicePlatform::createRenderWindow(const std::string &title, int width, int height)
{
    auto wnd = std::make_shared<Win32Window>(title, width, height);
    HDC hdc = wnd->getDeviceContext();
    auto ctx = std::make_shared<WGLContext>(hdc);
    auto gi = std::make_shared<OpenGLCoreGraphicsInterface>(std::move(ctx));
    auto rwnd = std::make_shared<RenderWindow>(std::move(wnd), std::move(gi));
    return rwnd;
}
