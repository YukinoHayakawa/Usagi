#include <Usagi/Engine/Extension/Vulkan/VulkanGraphicsDevice.hpp>
#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>
#include <Usagi/Engine/Runtime/Graphics/SwapChain.hpp>

using namespace yuki;

int main(int argc, char *argv[])
{
    std::shared_ptr<Win32Window> window = std::make_shared<Win32Window>("Usagi Vulkan Test", 1280, 720);
    std::shared_ptr<VulkanGraphicsDevice> graphics_device = std::make_shared<VulkanGraphicsDevice>();
    
    window->showWindow(true);
    auto swap_chain = graphics_device->createSwapChain(window);


    while(true)
    {
        window->processEvents();
        swap_chain->present();
        Sleep(10);
    }

    return 0;
}
