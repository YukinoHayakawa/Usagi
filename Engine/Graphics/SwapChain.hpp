#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class GpuImage;

class SwapChain : Noncopyable
{
public:
    virtual ~SwapChain() = default;

    virtual void acquireNextImage() = 0;
    virtual GpuImage * getCurrentImage() = 0;
};
}
