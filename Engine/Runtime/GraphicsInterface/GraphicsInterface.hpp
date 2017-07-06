#pragma once

namespace yuki
{

class GraphicsInterface
{
public:
    virtual ~GraphicsInterface() = default;

    virtual void swapBuffer() = 0;
    virtual void setCurrent() = 0;
    virtual void clear() = 0;
};

}
