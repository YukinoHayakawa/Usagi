#include <GL/glew.h>

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

void yuki::OpenGLCoreGraphicsInterface::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
