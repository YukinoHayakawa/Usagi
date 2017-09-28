#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{

class SwapChain : Noncopyable
{
public:
    virtual ~SwapChain() = default;

    // buffering mode
    // get framebuffers
    // recreate on window changing events
};

}
