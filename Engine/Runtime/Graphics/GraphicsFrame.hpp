#pragma once

#include <vector>

namespace yuki
{

class GraphicsFrame
{
public:
    virtual ~GraphicsFrame() = default;

    virtual void beginFrame(const std::vector<class GraphicsImage *> &attachments) = 0;

    // virtual class GraphicsCommandList * createCommandList(class GraphicsCommandPool *pool) = 0;
    virtual class GraphicsCommandList * getCommandList() = 0;
    virtual class GraphicsResourceAllocator * getResourceAllocator() = 0;
    virtual class GraphicsSemaphore * getRenderingFinishedSemaphore() = 0;

    virtual void endFrame() = 0;
};

}
