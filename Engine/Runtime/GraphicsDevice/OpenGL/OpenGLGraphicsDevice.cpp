#include <GL/glew.h>

#include "Context/OpenGLContext.hpp"

#include "OpenGLGraphicsDevice.hpp"
#include "OpenGLCommon.hpp"
#include "OpenGLVertexBuffer.hpp"
#include "OpenGLVertexShader.hpp"
#include "OpenGLFragmentShader.hpp"
#include "OpenGLPipeline.hpp"

yuki::OpenGLGraphicsDevice::OpenGLGraphicsDevice(std::shared_ptr<OpenGLContext> opengl_context)
    : mOpenGLContext { std::move(opengl_context) }
{
}

void yuki::OpenGLGraphicsDevice::setContextCurrent()
{
    mOpenGLContext->setCurrent();
}

std::shared_ptr<yuki::VertexBuffer> yuki::OpenGLGraphicsDevice::createVertexBuffer()
{
    return std::make_shared<OpenGLVertexBuffer>();
}

std::shared_ptr<yuki::VertexShader> yuki::OpenGLGraphicsDevice::createVertexShader()
{
    return std::make_shared<OpenGLVertexShader>();
}

std::shared_ptr<yuki::FragmentShader> yuki::OpenGLGraphicsDevice::createFragmentShader()
{
    return std::make_shared<OpenGLFragmentShader>();
}

std::shared_ptr<yuki::GDPipeline> yuki::OpenGLGraphicsDevice::createPipeline()
{
    return std::make_shared<OpenGLPipeline>();
}

void yuki::OpenGLGraphicsDevice::swapFrameBuffers()
{
    mOpenGLContext->swapBuffer();
}

void yuki::OpenGLGraphicsDevice::clearCurrentFrameBuffer()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void yuki::OpenGLGraphicsDevice::setScissorRect(int x, int y, int width, int height)
{
    // todo: scissor
}

void yuki::OpenGLGraphicsDevice::drawLines(size_t first, size_t count)
{
    _draw(GL_LINES, first, count);
}

void yuki::OpenGLGraphicsDevice::drawTriangles(size_t first, size_t count)
{
    _draw(GL_TRIANGLES, first, count);
}

void yuki::OpenGLGraphicsDevice::_draw(GLenum mode, size_t first, size_t count)
{
    glDrawArrays(mode, first, count);
    YUKI_OPENGL_CHECK();
}

void yuki::OpenGLGraphicsDevice::drawPoints(size_t first, size_t count)
{
    _draw(GL_POINTS, first, count);
}
