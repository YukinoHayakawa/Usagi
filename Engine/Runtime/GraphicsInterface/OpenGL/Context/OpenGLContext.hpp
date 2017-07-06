#pragma once

#include <Usagi/Engine/Utility/Exception.hpp>

namespace yuki
{

YUKI_DECL_RUNTIME_EXCEPTION(OpenGL);

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
