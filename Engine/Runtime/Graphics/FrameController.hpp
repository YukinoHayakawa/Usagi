#pragma once

#include "GraphicsFrame.hpp"
#include "GraphicsResourceAllocator.hpp"
#include "GraphicsMemoryAllocator.hpp"

namespace yuki
{

class FrameController
    : public GraphicsFrame
    , public GraphicsResourceAllocator
    , public GraphicsMemoryAllocator
{
protected:
    size_t mCurrentFrame = 0;
    void _swapFrame();

public:
    virtual size_t getFrameCount() const = 0;
    GraphicsResourceAllocator * getResourceAllocator() override;
};

}
