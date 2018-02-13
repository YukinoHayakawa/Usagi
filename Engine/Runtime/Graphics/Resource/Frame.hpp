#pragma once

#include <vector>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki::graphics
{

class Frame : Noncopyable
{
public:
    virtual ~Frame() = default;

    virtual void beginFrame(const std::vector<class Image *> &attachments) = 0;

    // virtual class GraphicsCommandList * createCommandList(class GraphicsCommandPool *pool) = 0;
    // todo multiple command list
    virtual class CommandList * getCommandList() = 0;
    virtual class Waitable * getRenderingFinishedSemaphore() = 0;
    virtual class Waitable * getRenderingFinishedFence() = 0;

    virtual void endFrame() = 0;
};

}
