#pragma once

#include "Frame.hpp"
#include "ResourceAllocator.hpp"
#include "MemoryAllocator.hpp"

namespace yuki::graphics
{

class FrameController
    : public Frame
    , public ResourceAllocator
    , public MemoryAllocator
{
protected:
    size_t mCurrentFrame = 0;
    void _swapFrame();

public:
    virtual size_t getFrameCount() const = 0;
    ResourceAllocator * getResourceAllocator() override;
};

}
