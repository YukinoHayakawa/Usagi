#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Filesystem/FilesystemAssetPackage.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Game/SingleWindowGame.hpp>
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

#include <Usagi/Extension/ImGui/ImGui.hpp>
#include <Usagi/Extension/ImGui/ImGuiComponent.hpp>
#include <Usagi/Extension/ImGui/ImGuiSubsystem.hpp>
#include <imgui/imgui_demo.cpp>

using namespace usagi;

struct ImGuiDemoComponent : ImGuiComponent
{
    void draw(const TimeDuration &dt) override
    {
        ImGui::ShowDemoWindow();
        ImGui::ShowMetricsWindow();
    }
};

class ImGuiDemo : public SingleWindowGame
{
    ImGuiSubsystem *mImGui = nullptr;
    RenderPassCreateInfo mAttachments;
    Element *mImGuiRoot = nullptr;

public:
    explicit ImGuiDemo(Runtime *runtime)
        : SingleWindowGame {
            runtime,
            u8"🐰 - ImGui Demo",
            Vector2i { 100, 100 },
            Vector2u32 { 1920, 1080 }
        }
    {
        // Setting up ImGui
        assets()->addChild<FilesystemAssetPackage>(
            runtime, "imgui", "Data/imgui");
        const auto input_manager = runtime->inputManager();
        mImGui = addSubsystem("imgui", std::make_unique<ImGuiSubsystem>(
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
        mImGui->createPipeline(mAttachments);

        mImGuiRoot = rootElement()->addChild("ImGuiRoot");
        mImGuiRoot->addComponent<ImGuiDemoComponent>();
    }

    void run()
    {
        using Clock = std::chrono::high_resolution_clock;
        if constexpr(!Clock::is_steady)
            LOG(warn, "std::chrono::high_resolution_clock is not steady!");

        auto start = Clock::now();
        TimeDuration dt { 0 };
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

            mImGui->setRenderSizes(mWindow->size(), framebuffer->size());

            update(dt);

            // Record command lists
            std::vector<std::shared_ptr<GraphicsCommandList>> cmd_lists;
            const auto cmd_inserter = [&](auto cmd_list) {
                cmd_lists.push_back(std::move(cmd_list));
            };
            mImGui->render(dt, framebuffer, cmd_inserter);

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
            const auto end = Clock::now();
            dt = std::chrono::duration_cast<TimeDuration>(end - start);
            start = end;
        }
    }
};

int main(int argc, char *argv[])
{
    auto runtime = Runtime::create();
    runtime->enableCrashHandler("ImGuiDemoErrorDump");
    try
    {
        ImGuiDemo demo(runtime.get());
        demo.run();
    }
    catch(const std::exception &e)
    {
        runtime->displayErrorDialog(e.what());
        throw;
    }
}
