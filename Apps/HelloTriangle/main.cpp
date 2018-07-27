#include <Usagi/Engine/Runtime/Runtime.hpp>
#include <Usagi/Engine/Runtime/Window/Window.hpp>
#include <Usagi/Engine/Runtime/Graphics/SwapChain.hpp>
#include <Usagi/Engine/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Engine/Runtime/Window/WindowManager.hpp>

using namespace usagi;

class HelloTriangle
{
    Runtime mRuntime;
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<SwapChain> mSwapChain;

public:
    HelloTriangle()
    {
        mRuntime.initWindow();
        mRuntime.initGpu();

        mWindow = mRuntime.windowManager()->createWindow(
            u8"\U0001F430 - Hello Triangle",
            Vector2i{ 100, 100 },
            Vector2u32{ 1920, 1080 }
        );
        mSwapChain = mRuntime.gpu()->createSwapChain(mWindow.get());
    }

    void mainLoop()
    {
        while(mWindow->isOpen())
        {
            mRuntime.windowManager()->processEvents();
        }
    }
};

int main(int argc, char *argv[])
{
    HelloTriangle app;
    app.mainLoop();
}
