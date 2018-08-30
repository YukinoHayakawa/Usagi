#include "SingleWindowGame.hpp"

#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Window/WindowManager.hpp>
#include <Usagi/Runtime/Window/Window.hpp>
#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>

usagi::SingleWindowGame::SingleWindowGame(
    Runtime *runtime,
    const std::string &window_title,
    const Vector2i &window_position,
    const Vector2u32 &window_size,
    const GpuBufferFormat swapchain_format)
    : Game(runtime)
{
    // Setting up window
    mRuntime->initWindow();
    mWindow = mRuntime->windowManager()->createWindow(
        window_title, window_position, window_size);
    mWindow->addEventListener(this);

    // Setting up graphics
    mRuntime->initGpu();

    mSwapchain = mRuntime->gpu()->createSwapchain(mWindow.get());
    mSwapchain->create(mWindow->size(), swapchain_format);

    // Setting up input
    mRuntime->initInput();
}

usagi::SingleWindowGame::~SingleWindowGame()
{
    mWindow->removeEventListener(this);
}

void usagi::SingleWindowGame::onWindowResizeEnd(const WindowSizeEvent &e)
{
    if(e.size.x() != 0 && e.size.y() != 0)
        mSwapchain->resize(e.size);
}
