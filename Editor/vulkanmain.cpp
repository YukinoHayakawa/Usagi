#include <Usagi/Engine/Extension/Vulkan/Device/Device.hpp>
#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>
#include <Usagi/Engine/Runtime/Graphics/Device/SwapChain.hpp>
#include <Usagi/Engine/Runtime/Graphics/Workload/CommandPool.hpp>
#include <Usagi/Engine/Runtime/Graphics/Workload/CommandList.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/FrameController.hpp>
#include <Usagi/Engine/Runtime/Graphics/Environment.hpp>
#include <Usagi/Engine/Time/Clock.hpp>
#include "VulkanTriangle.hpp"

using namespace yuki;

int main(int argc, char *argv[])
{
    graphics::Environment env;
    env.graphics_device = std::make_unique<vulkan::Device>();
    env.window = std::make_unique<Win32Window>("Vulkan Test", 1280, 720);
    env.swap_chain = env.graphics_device->createSwapChain(env.window.get());
    env.frame_control = env.graphics_device->createFrameController(3);

    VulkanTriangle triangle(&env);

    Clock clock;
    size_t frame_count = 0;

    while(env.window->isWindowOpen())
    {
        double delta_time = clock.tick();

        env.window->processEvents();
        triangle.update(delta_time);

        env.swap_chain->acquireNextImage();
        env.frame_control->beginFrame({ env.swap_chain->getCurrentImage() });

        auto command_list = env.frame_control->getCommandList();
        command_list->begin();
        triangle.populateCommandList(command_list);
        command_list->end();

        env.graphics_device->submitGraphicsCommandList(
            command_list,
            { env.swap_chain->getImageAvailableSemaphore() },
            { env.frame_control->getRenderingFinishedSemaphore() },
            env.frame_control->getRenderingFinishedFence()
        );
        env.swap_chain->present({ env.frame_control->getRenderingFinishedSemaphore() });

        env.frame_control->endFrame();

        ++frame_count;
    }

    // must wait for the device to be idle before releasing resources
    env.graphics_device->waitIdle();

    return 0;
}
