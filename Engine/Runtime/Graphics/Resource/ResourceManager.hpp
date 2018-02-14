#pragma once

#include <memory>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

#include "DynamicBuffer.hpp"

namespace yuki::graphics
{
class ResourceManager : Noncopyable
{
public:
    virtual ~ResourceManager() = default;

    virtual std::unique_ptr<class DynamicBuffer> createDynamicBuffer(std::size_t size,
        DynamicBuffer::UsageFlag usage) = 0;
};
}
