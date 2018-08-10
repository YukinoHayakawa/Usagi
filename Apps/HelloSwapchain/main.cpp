#include <Usagi/Engine/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Engine/Runtime/Graphics/RenderPassCreateInfo.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuCommandPool.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GraphicsCommandList.hpp>
#include <Usagi/Engine/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Engine/Runtime/Input/InputManager.hpp>
#include <Usagi/Engine/Runtime/Input/Keyboard/Keyboard.hpp>
#include <Usagi/Engine/Runtime/Input/Keyboard/KeyEventListener.hpp>
#include <Usagi/Engine/Runtime/Runtime.hpp>
#include <Usagi/Engine/Runtime/Window/Window.hpp>
#include <Usagi/Engine/Runtime/Window/WindowManager.hpp>

using namespace usagi;

class HelloSwapchain
    : public KeyEventListener
    , public WindowEventListener
{
    Runtime mRuntime;
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Swapchain> mSwapChain;
    std::shared_ptr<GpuCommandPool> mCommandPool;
    Color4f mFillColor = Color4f::Random();

public:
    HelloSwapchain()
    {
        // Setting up window
        mRuntime.initWindow();
        mWindow = mRuntime.windowManager()->createWindow(
            u8"🐰 - Hello Swapchain",
            Vector2i { 100, 100 },
            Vector2u32 { 1920, 1080 }
        );
        mWindow->addEventListener(this);

        // Setting up graphics
        mRuntime.initGpu();

        mSwapChain = mRuntime.gpu()->createSwapchain(mWindow.get());
        mSwapChain->create(mWindow->size(), GpuBufferFormat::R8G8B8A8_UNORM);

        // todo pool is released before command buffers on shutdown
        // use sharedptr in tree like structures
        mCommandPool = mRuntime.gpu()->createCommandPool();

        // Setting up input
        mRuntime.initInput();
        mRuntime.inputManager()->virtualKeyboard()->addEventListener(this);
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

    void onWindowResizeEnd(const WindowSizeEvent &e) override
    {
        if(e.size.x() != 0 && e.size.y() != 0)
            mSwapChain->resize(e.size);
    }

    void mainLoop()
    {
        auto gpu = mRuntime.gpu();

        while(mWindow->isOpen())
        {
            mRuntime.windowManager()->processEvents();
            mRuntime.inputManager()->processEvents();
            mSwapChain->acquireNextImage();

            auto cmd_list = mCommandPool->allocateGraphicsCommandList();
            cmd_list->beginRecording();
            cmd_list->transitionImage(
                mSwapChain->currentImage(),
                GpuImageLayout::UNDEFINED,
                GpuImageLayout::TRANSFER_DST,
                GraphicsPipelineStage::TRANSFER,
                GraphicsPipelineStage::TRANSFER,
                GpuAccess::MEMORY_READ,
                GpuAccess::TRANSFER_WRITE
            );
            cmd_list->clearColorImage(
                mSwapChain->currentImage(), mFillColor
            );
            cmd_list->transitionImage(
                mSwapChain->currentImage(),
                GpuImageLayout::TRANSFER_DST,
                GpuImageLayout::PRESENT,
                GraphicsPipelineStage::TRANSFER,
                GraphicsPipelineStage::BOTTOM_OF_PIPE,
                GpuAccess::TRANSFER_WRITE,
                GpuAccess::MEMORY_READ
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
