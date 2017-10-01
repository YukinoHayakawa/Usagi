#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{

class SwapChain : Noncopyable
{
public:
    virtual ~SwapChain() = default;

    virtual void present() = 0;

    // buffering mode
    // get framebuffers
    // recreate on window changing events
};

}
