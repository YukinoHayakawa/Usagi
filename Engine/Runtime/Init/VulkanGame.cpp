#include <Usagi/Engine/Game/Game.hpp>

#include <Usagi/Engine/Extension/Vulkan/VulkanGpuDevice.hpp>

void usagi::Game::initializeGraphics()
{
    mGpuDevice = std::make_unique<VulkanGpuDevice>();
}
