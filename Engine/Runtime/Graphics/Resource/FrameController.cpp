#include "FrameController.hpp"

namespace yuki::graphics
{
void FrameController::_swapFrame()
{
    mCurrentFrame = (mCurrentFrame + 1) % getFrameCount();
}
}
