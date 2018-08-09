#include <Usagi/Engine/Runtime/Runtime.hpp>
#include <Usagi/Engine/Runtime/Window/Window.hpp>
#include <Usagi/Engine/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Engine/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Engine/Runtime/Window/WindowManager.hpp>
#include <Usagi/Engine/Runtime/Input/Keyboard/KeyEventListener.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuCommandPool.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GraphicsCommandList.hpp>

using namespace usagi;

class HelloSwapchain
    : public KeyEventListener
{
    Runtime mRuntime;
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Swapchain> mSwapChain;
    std::shared_ptr<GpuCommandPool> mCommandPool;
    Color4f mFillColor = Color4f::Random();

public:
    HelloSwapchain()
    {
        mRuntime.initWindow();
        mRuntime.initGpu();

        mWindow = mRuntime.windowManager()->createWindow(
            u8"🐰 - Hello Swapchain",
            Vector2i { 100, 100 },
            Vector2u32 { 1920, 1080 }
        );
        mSwapChain = mRuntime.gpu()->createSwapchain(mWindow.get());
        mSwapChain->create(GpuBufferFormat::R8G8B8A8_UNORM);

        mCommandPool = mRuntime.gpu()->createCommandPool();
    }

    void onKeyStateChange(const KeyEvent &e) override
    {
        switch(e.key_code)
        {
            case KeyCode::ENTER:
                if(e.pressed)
                    mFillColor = Color4f::Random();
                break;
            default: break ;
        }
    }

    void mainLoop()
    {
        while(mWindow->isOpen())
        {
            auto gpu = mRuntime.gpu();

            mRuntime.windowManager()->processEvents();
            mSwapChain->acquireNextImage();

            auto cmd_list = mCommandPool->allocateGraphicsCommandList();
            cmd_list->beginRecording();
            cmd_list->clearColorImage(
                mSwapChain->currentImage(), mFillColor
            );
            cmd_list->endRecording();

            const auto rendering_finished_sem = gpu->createSemaphore();
            gpu->submitGraphicsJobs(
                { cmd_list }, { }, { }, { rendering_finished_sem }
            );
            mSwapChain->present({ rendering_finished_sem });

            gpu->reclaimResources();
        }
    }
};

int main(int argc, char *argv[])
{
    HelloSwapchain app;
    app.mainLoop();
}
