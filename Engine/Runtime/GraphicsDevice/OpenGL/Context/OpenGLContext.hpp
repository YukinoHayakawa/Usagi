#pragma once

namespace yuki
{


/**
 * \brief Create platform-specific OpenGL context.
 */
class OpenGLContext
{
public:
    virtual ~OpenGLContext()
    {
    }

    virtual void setCurrent() = 0;
    virtual void swapBuffer() = 0;
};

}
