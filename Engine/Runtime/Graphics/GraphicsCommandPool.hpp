#pragma once

#include <memory>

namespace yuki
{

class GraphicsCommandPool
{
public:
    virtual ~GraphicsCommandPool() = default;

    virtual std::unique_ptr<class GraphicsCommandList> createCommandList() = 0;
};

}
