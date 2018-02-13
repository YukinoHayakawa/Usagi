#include "DynamicBuffer.hpp"

namespace yuki::extension::vulkan
{
DynamicBuffer::DynamicBuffer(ResourceManager *resource_manager, std::size_t size)
    : mResourceManager { resource_manager }
    , mSize { size }
{
}
}
