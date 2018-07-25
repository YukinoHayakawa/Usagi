#include <Usagi/Engine/Game/Game.hpp>
#include <Usagi/Engine/Runtime/Platform.hpp>
#include <Usagi/Engine/Runtime/Window/Window.hpp>
#include <Usagi/Engine/Graphics/SwapChain.hpp>
#include <Usagi/Engine/Graphics/GpuDevice.hpp>

using namespace usagi;
using namespace std::chrono_literals;

class HelloTriangle
{
    Game mGame;
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<SwapChain> mSwapChain;

public:
    HelloTriangle()
    {
        mGame.initializeGraphics();

        mWindow = mGame.platform()->createWindow(
            u8"\U0001F430 - Hello Triangle",
            Vector2i{ 100, 100 },
            Vector2u32{ 1920, 1080 }
        );
        mSwapChain = mGame.gpuDevice()->createSwapChain(mWindow.get());
    }

    void mainLoop()
    {
        while(mWindow->isOpen())
        {
            mGame.update(0s);
        }
    }
};

int main(int argc, char *argv[])
{
    HelloTriangle app;
    app.mainLoop();
}
