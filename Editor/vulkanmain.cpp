#include <Usagi/Engine/Extension/Vulkan/VulkanGraphicsDevice.hpp>
#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>
#include <Usagi/Engine/Runtime/Graphics/SwapChain.hpp>
#include <Usagi/Engine/Runtime/Graphics/GraphicsCommandPool.hpp>
#include <Usagi/Engine/Runtime/Graphics/FrameController.hpp>
#include <Usagi/Engine/Runtime/Graphics/GraphicsCommandList.hpp>
#include <Usagi/Engine/Runtime/Graphics/GraphicsEnvironment.hpp>
#include <Usagi/Engine/Time/Clock.hpp>
#include "VulkanTriangle.hpp"

using namespace yuki;

int main(int argc, char *argv[])
{
    GraphicsEnvironment env;
    env.graphics_device = std::make_unique<VulkanGraphicsDevice>();
    env.window = std::make_unique<Win32Window>("Vulkan Test", 1280, 720);
    env.swap_chain = env.graphics_device->createSwapChain(env.window.get());
    env.frame_control = env.graphics_device->createFrameController(2);

    VulkanTriangle triangle(&env);

    Clock clock;
    while(env.window->isWindowOpen())
    {
        env.window->processEvents();
        triangle.update(clock.getElapsedTime());

        env.frame_control->beginFrame({ env.swap_chain->getCurrentImage() });
        env.swap_chain->acquireNextImage();

        auto command_list = env.frame_control->getCommandList();
        command_list->begin();
        triangle.populateCommandList(command_list);
        command_list->end();

        env.graphics_device->submitGraphicsCommandList(
            command_list,
            { env.swap_chain->getImageAvailableSemaphore() },
            { env.frame_control->getRenderingFinishedSemaphore() }
        );
        env.swap_chain->present({ env.frame_control->getRenderingFinishedSemaphore() });

        env.frame_control->endFrame();

        clock.tick();
    }

    return 0;
}
