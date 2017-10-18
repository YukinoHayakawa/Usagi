#include "FrameController.hpp"

yuki::GraphicsResourceAllocator * yuki::FrameController::getResourceAllocator()
{
    return this;
}

void yuki::FrameController::_swapFrame()
{
    mCurrentFrame = (mCurrentFrame + 1) % getFrameCount();
}
