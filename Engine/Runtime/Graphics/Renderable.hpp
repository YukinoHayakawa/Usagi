#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki::graphics
{

class Renderable : Noncopyable
{
public:
    virtual ~Renderable() = default;

    virtual void update(double delta_time) = 0;
    virtual void populateCommandList(class CommandList *command_list) = 0;
};

}
