#include <Usagi/Engine/Extension/Vulkan/Device/Device.hpp>
#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>
#include <Usagi/Engine/Runtime/Graphics/Device/SwapChain.hpp>
#include <Usagi/Engine/Runtime/Graphics/Workload/CommandPool.hpp>
#include <Usagi/Engine/Runtime/Graphics/Workload/CommandList.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/FrameController.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/ResourceManager.hpp>
#include <Usagi/Engine/Runtime/Graphics/Environment.hpp>
#include <Usagi/Engine/Time/Clock.hpp>
#include "VulkanTriangle.hpp"

using namespace yuki;
using namespace extension;

int main(int argc, char *argv[])
{
    graphics::Environment env;
    env.graphics_device = std::make_unique<vulkan::Device>();
    env.window = std::make_unique<Win32Window>("Usagi Vulkan Demo", 1280, 720);
    env.swap_chain = env.graphics_device->createSwapChain(env.window.get());
    env.resource_manager = env.graphics_device->createResourceManager();
    env.frame_controller = env.graphics_device->createFrameController(3);

    VulkanTriangle triangle(&env);

    Clock clock; // todo use chrono::duration
    size_t frame_count = 0;

    // todo make a fps counter
    double frame_time_accumulator = 0;
    std::size_t fps_sample_count = 0;

    while(env.window->isWindowOpen())
    {
        double delta_time = clock.tick();

        // calculate fps
        frame_time_accumulator += delta_time;
        ++fps_sample_count;

        if(frame_time_accumulator > 0.25)
        {
            std::stringstream title_builder;
            title_builder << "Usagi Vulkan Demo (" <<
                fps_sample_count / frame_time_accumulator << " fps)";
            env.window->setTitle(title_builder.str());
            frame_time_accumulator = 0;
            fps_sample_count = 0;
        }

        // receive user input
        env.window->processEvents();

        // update 
        triangle.update(delta_time);

        // todo command recording should not wait on next acquiring image, use a pool of command list
        // todo extensive abstraction leakage below
        env.swap_chain->acquireNextImage();
        env.frame_controller->beginFrame({ env.swap_chain->getCurrentImage() });

        auto command_list = env.frame_controller->getCommandList();
        command_list->begin();
        triangle.populateCommandList(command_list);
        command_list->end();


        // todo: this literally cannot be ported to dx12
        env.graphics_device->submitGraphicsCommandList(
            command_list,
            { env.swap_chain->getImageAvailableSemaphore() },
            { env.frame_controller->getRenderingFinishedSemaphore() },
            env.frame_controller->getRenderingFinishedFence()
        );
        env.swap_chain->present({
            env.frame_controller->getRenderingFinishedSemaphore()
        });

        env.frame_controller->endFrame();

        ++frame_count;
    }
    // todo: fix streaming first.

    // must wait for the device to be idle before releasing resources
    env.graphics_device->waitIdle();

    return 0;
}
