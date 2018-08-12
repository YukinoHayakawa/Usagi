#include <Usagi/Engine/Core/Logging.hpp>
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
    : public KeyEventListener,
    public WindowEventListener
{
    Runtime mRuntime;
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Swapchain> mSwapchain;
    std::shared_ptr<GpuCommandPool> mCommandPool;
    Color4f mFillColor { 0.64f, 0.81f, 1.0f, 1.0f }; // baby blue

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

        mSwapchain = mRuntime.gpu()->createSwapchain(mWindow.get());
        mSwapchain->create(mWindow->size(), GpuBufferFormat::R8G8B8A8_UNORM);

        mCommandPool = mRuntime.gpu()->createCommandPool();

        // Setting up input
        mRuntime.initInput();
        mRuntime.inputManager()->virtualKeyboard()->addEventListener(this);
    }

    void onKeyStateChange(const KeyEvent &e) override
    {
        if(!e.pressed) return;
        switch(e.key_code)
        {
            case KeyCode::ENTER:
                (mFillColor = Color4f::Random()).w() = 1;
                LOG(info, "Random");
                break;
            case KeyCode::BACKQUOTE:
                mFillColor = { 1, 0.82f, 0.87f, 1 };
                LOG(info, "Light Pink");
                break;
            case KeyCode::DIGIT_1:
                mFillColor = { 1, 0, 0, 1 };
                LOG(info, "Red");
                break;
            case KeyCode::DIGIT_2:
                mFillColor = { 0, 1, 0, 1 };
                LOG(info, "Green");
                break;
            case KeyCode::DIGIT_3:
                mFillColor = { 0, 0, 1, 1 };
                LOG(info, "Blue");
                break;
            case KeyCode::DIGIT_4:
                mFillColor = { 1, 1, 0, 1 };
                LOG(info, "Yellow");
                break;
            case KeyCode::DIGIT_5:
                mFillColor = { 1, 0, 1, 1 };
                LOG(info, "Magenta");
                break;
            case KeyCode::DIGIT_6:
                mFillColor = { 0, 1, 1, 1 };
                LOG(info, "Aqua");
                break;
            case KeyCode::DIGIT_7:
                mFillColor = { 1, 1, 1, 1 };
                LOG(info, "White");
                break;
            case KeyCode::DIGIT_8:
                mFillColor = { 0, 0, 0, 1 };
                LOG(info, "Black");
                break;
            default: break ;
        }
    }

    void onWindowResizeEnd(const WindowSizeEvent &e) override
    {
        if(e.size.x() != 0 && e.size.y() != 0)
            mSwapchain->resize(e.size);
    }

    void mainLoop()
    {
        auto gpu = mRuntime.gpu();

        while(mWindow->isOpen())
        {
            mRuntime.windowManager()->processEvents();
            mRuntime.inputManager()->processEvents();
            mSwapchain->acquireNextImage();

            auto cmd_list = mCommandPool->allocateGraphicsCommandList();
            cmd_list->beginRecording();
            cmd_list->transitionImage(
                mSwapchain->currentImage(),
                GpuImageLayout::UNDEFINED,
                GpuImageLayout::TRANSFER_DST,
                GraphicsPipelineStage::TRANSFER,
                GraphicsPipelineStage::TRANSFER
            );
            cmd_list->clearColorImage(
                mSwapchain->currentImage(),
                GpuImageLayout::TRANSFER_DST,
                mFillColor
            );
            cmd_list->transitionImage(
                mSwapchain->currentImage(),
                GpuImageLayout::TRANSFER_DST,
                GpuImageLayout::PRESENT,
                GraphicsPipelineStage::TRANSFER,
                GraphicsPipelineStage::BOTTOM_OF_PIPE
            );
            cmd_list->endRecording();

            const auto rendering_finished_sem = gpu->createSemaphore();
            gpu->submitGraphicsJobs(
                { cmd_list }, { }, { }, { rendering_finished_sem }
            );
            mSwapchain->present({ rendering_finished_sem });

            gpu->reclaimResources();
        }
    }
};

int main(int argc, char *argv[])
{
    HelloSwapchain app;
    app.mainLoop();
}
