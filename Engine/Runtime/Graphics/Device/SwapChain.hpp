#pragma once

#include <vector>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki::graphics
{

class Waitable;
class Image;

class SwapChain : Noncopyable
{
public:
    virtual ~SwapChain() = default;

    virtual void acquireNextImage() = 0;
    virtual Image * getCurrentImage() = 0;
    virtual void present(const std::vector<Waitable *> &wait_gpu_events) = 0;

    virtual Waitable * getImageAvailableSemaphore() = 0;

    virtual uint32_t getNativeImageFormat() = 0;

    // buffering mode
    // get framebuffers
    // recreate on window changing events
};

}
