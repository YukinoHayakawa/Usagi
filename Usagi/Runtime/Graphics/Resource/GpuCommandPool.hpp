#pragma once

#include <memory>

#include <Usagi/Utility/Noncopyable.hpp>

namespace usagi
{
class GraphicsCommandList;

class GpuCommandPool : Noncopyable
{
public:
    virtual ~GpuCommandPool() = default;

    virtual std::shared_ptr<GraphicsCommandList> allocateGraphicsCommandList()
        = 0;
};
}
