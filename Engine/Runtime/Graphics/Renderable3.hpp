#pragma once

namespace yuki
{

class Renderable3
{
public:
    virtual ~Renderable3() = default;

    virtual void update(double delta_time) = 0;
    virtual void populateCommandList(class GraphicsCommandList *command_list) = 0;
};

}
