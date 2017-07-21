#pragma once

#include <functional>

namespace yuki
{

class BindingSentry
{
    const std::function<void()> unbind;

public:
    BindingSentry(std::function<void()> unbind)
        : unbind { std::move(unbind) }
    {
    }

    ~BindingSentry()
    {
        if(unbind) unbind();
    }
};

}
