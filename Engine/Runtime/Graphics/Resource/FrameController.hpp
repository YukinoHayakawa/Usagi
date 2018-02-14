#pragma once

#include "Frame.hpp"

namespace yuki::graphics
{

class FrameController : public Frame
{
protected:
    size_t mCurrentFrame = 0;
    void _swapFrame();

public:
    virtual size_t getFrameCount() const = 0;
};

}
