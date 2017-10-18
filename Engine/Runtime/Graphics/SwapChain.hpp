#pragma once

#include <cstdint>
#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{

class SwapChain : Noncopyable
{
public:
    virtual ~SwapChain() = default;

    virtual void acquireNextImage() = 0;
    virtual class GraphicsImage * getCurrentImage() = 0;
    virtual void present(const std::vector<GraphicsSemaphore *> wait_semaphores) = 0;

    virtual GraphicsSemaphore * getImageAvailableSemaphore() = 0;

    virtual uint32_t getNativeImageFormat() = 0;

    // buffering mode
    // get framebuffers
    // recreate on window changing events
};

}
