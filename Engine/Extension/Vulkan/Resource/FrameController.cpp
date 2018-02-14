#include <Usagi/Engine/Runtime/Exception.hpp>

#include "FrameController.hpp"
#include "Frame.hpp"

namespace yuki::extension::vulkan
{
Frame * FrameController::_getCurrentFrame()
{
    return mFrameChain[mCurrentFrame].get();
}

FrameController::FrameController(Device *device, size_t num_frames)
    : mDevice { device }
{
    if(num_frames == 0) throw InvalidArgumentException();
    mFrameChain.reserve(num_frames);
    for(size_t i = 0; i < num_frames; ++i)
    {
        mFrameChain.emplace_back(std::make_unique<vulkan::Frame>(mDevice));
    }
}

size_t FrameController::getFrameCount() const
{
    return mFrameChain.size();
}

void FrameController::beginFrame(const std::vector<class graphics::Image *> &attachments)
{
    _getCurrentFrame()->beginFrame(attachments);
}

graphics::CommandList * FrameController::getCommandList()
{
    return _getCurrentFrame()->getCommandList();
}

graphics::Waitable * FrameController::getRenderingFinishedSemaphore()
{
    return _getCurrentFrame()->getRenderingFinishedSemaphore();
}

graphics::Waitable * FrameController::getRenderingFinishedFence()
{
    return _getCurrentFrame()->getRenderingFinishedFence();
}

void FrameController::endFrame()
{
    _getCurrentFrame()->endFrame();
    _swapFrame();
}
}
