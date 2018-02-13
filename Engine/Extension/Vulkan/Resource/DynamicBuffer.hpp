#pragma once

#include <Usagi/Engine/Runtime/Graphics/Resource/DynamicBuffer.hpp>

#include "detail/PrimitiveBuffer.hpp"
#include "detail/Buffer.hpp"

namespace yuki::extension::vulkan
{
class DynamicBuffer : public graphics::DynamicBuffer, public Buffer
{
protected:
    class ResourceManager *mResourceManager = nullptr;

    std::size_t mSize = 0;

public:
    DynamicBuffer(ResourceManager *resource_manager, std::size_t size);

    std::size_t size() const override { return mSize; }
};
}
