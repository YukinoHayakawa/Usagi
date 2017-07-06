#include "Context/OpenGLContext.hpp"

#include "OpenGLCoreGraphicsInterface.hpp"

yuki::OpenGLCoreGraphicsInterface::OpenGLCoreGraphicsInterface(std::shared_ptr<OpenGLContext> opengl_context)
    : mOpenGLContext { std::move(opengl_context) }
{
}

void yuki::OpenGLCoreGraphicsInterface::swapBuffer()
{
    mOpenGLContext->swapBuffer();
}

void yuki::OpenGLCoreGraphicsInterface::setCurrent()
{
    mOpenGLContext->setCurrent();
}
