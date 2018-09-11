#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Package/Filesystem/FilesystemAssetPackage.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Game/GraphicalGame.hpp>
#include <Usagi/Runtime/Graphics/Enum/GraphicsPipelineStage.hpp>
#include <Usagi/Runtime/Graphics/Framebuffer.hpp>
#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>
#include <Usagi/Runtime/Graphics/GraphicsCommandList.hpp>
#include <Usagi/Runtime/Graphics/RenderPassCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Runtime/Input/InputManager.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Window/Window.hpp>
#include <Usagi/Runtime/Window/WindowManager.hpp>

#include <Usagi/Extension/Nuklear/Nuklear.hpp>
#include <Usagi/Extension/Nuklear/NuklearComponent.hpp>
#include <Usagi/Extension/Nuklear/NuklearSubsystem.hpp>

#include "style.c"
#include "calculator.c"
#include "overview.c"
#include "node_editor.c"

using namespace usagi;

struct NuklearDemoComponent : NuklearComponent
{
    void draw(const Clock &clock, nk_context *ctx) override
    {
        nk_colorf bg;
        bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
        if(nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
            NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
        {
            enum { EASY, HARD };
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(ctx, 30, 80, 1);
            if(nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(ctx, 30, 2);
            if(nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if(nk_option_label(ctx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(ctx, 22, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 400)))
            {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);
        calculator(ctx);
        overview(ctx);
        node_editor(ctx);
    }
};

class NuklearDemo : public GraphicalGame
{
    NuklearSubsystem *mNuklear= nullptr;
    RenderPassCreateInfo mAttachments;
    Element *mNuklearRoot = nullptr;

public:
    explicit NuklearDemo(Runtime *runtime)
        : GraphicalGame {
            runtime,
            u8"🐰 - Nuklear Demo",
            Vector2i { 100, 100 },
            Vector2u32 { 1920, 1080 }
        }
    {
        // Setting up ImGui
        assets()->addChild<FilesystemAssetPackage>("nuklear", "Data/nuklear");
        const auto input_manager = runtime->inputManager();
        mNuklear = addSubsystem("nuklear", std::make_unique<NuklearSubsystem>(
            this,
            mWindow,
            input_manager->virtualKeyboard(),
            input_manager->virtualMouse()
        ));
        mAttachments.attachment_usages.emplace_back(
            mSwapchain->format(),
            1,
            GpuImageLayout::UNDEFINED,
            GpuImageLayout::PRESENT,
            GpuAttachmentLoadOp::CLEAR,
            GpuAttachmentStoreOp::STORE
        ).clear_color = Color4f::Zero();
        mNuklear->createPipeline(mAttachments);

        mNuklearRoot = rootElement()->addChild("NuklearRoot");
        mNuklearRoot->addComponent<NuklearDemoComponent>();
    }

    void run()
    {
        while(mWindow->isOpen())
        {
            auto gpu = runtime()->gpu();

            // Process window/input events
            runtime()->windowManager()->processEvents();
            runtime()->inputManager()->processEvents();

            // Create framebuffer
            const auto wait_semaphores = { mSwapchain->acquireNextImage() };
            const auto framebuffer = gpu->createFramebuffer(
                mSwapchain->size(),
                {
                    mSwapchain->currentImage()->baseView()
                }
            );

            mNuklear->setRenderSizes(mWindow->size(), framebuffer->size());

            update(mMasterClock);

            // Record command lists
            std::vector<std::shared_ptr<GraphicsCommandList>> cmd_lists;
            const auto cmd_inserter = [&](auto cmd_list) {
                cmd_lists.push_back(std::move(cmd_list));
            };
            mNuklear->render(mMasterClock, framebuffer, cmd_inserter);

            // Submit jobs
            const auto wait_stages = {
                GraphicsPipelineStage::COLOR_ATTACHMENT_OUTPUT
            };
            const auto rendering_finished_sem = gpu->createSemaphore();
            const auto signal_semaphores = {
                rendering_finished_sem
            };
            gpu->submitGraphicsJobs(
                cmd_lists,
                wait_semaphores,
                wait_stages,
                signal_semaphores
            );

            // Present image
            mSwapchain->present({ rendering_finished_sem });

            // GC
            gpu->reclaimResources();

            // Calculate elapsed time
            mMasterClock.tick();
        }
    }
};

int main(int argc, char *argv[])
{
    auto runtime = Runtime::create();
    runtime->enableCrashHandler("NuklearDemoErrorDump");
    try
    {
        NuklearDemo demo(runtime.get());
        demo.run();
    }
    catch(const std::exception &e)
    {
        runtime->displayErrorDialog(e.what());
        throw;
    }
}
