#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class VulkanBatchResource : Noncopyable
{
public:
    virtual ~VulkanBatchResource() = default;
};
}
