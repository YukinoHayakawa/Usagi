#pragma once

#include <memory>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki::graphics
{

class CommandPool : Noncopyable
{
public:
    virtual ~CommandPool() = default;

    virtual std::unique_ptr<class CommandList> createCommandList() = 0;
};

}
