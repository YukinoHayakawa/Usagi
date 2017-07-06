#pragma once

#include <memory>

#include "../GraphicsInterface.hpp"

namespace yuki
{

class OpenGLContext;

class OpenGLCoreGraphicsInterface : public GraphicsInterface
{
    std::shared_ptr<OpenGLContext> mOpenGLContext;

public:
    OpenGLCoreGraphicsInterface(std::shared_ptr<OpenGLContext> opengl_context);
};

}
