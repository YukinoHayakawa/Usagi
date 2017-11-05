#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Device/Waitable.hpp>

namespace yuki::vulkan
{

class Fence : public graphics::Waitable
{
public:
    vk::Fence fence;
};

}
