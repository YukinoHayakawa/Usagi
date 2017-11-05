#include "FrameController.hpp"

namespace yuki::graphics
{

ResourceAllocator * FrameController::getResourceAllocator()
{
    return this;
}

void FrameController::_swapFrame()
{
    mCurrentFrame = (mCurrentFrame + 1) % getFrameCount();
}

}
