#include <GL/glew.h>

#include "Context/OpenGLContext.hpp"

#include "OpenGLGraphicsDevice.hpp"
#include "OpenGLCommon.hpp"
#include "OpenGLTexture.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLRasterizerState.hpp"
#include "OpenGLBlendState.hpp"
#include "OpenGLVertexBuffer.hpp"

yuki::OpenGLGraphicsDevice::OpenGLGraphicsDevice(std::shared_ptr<OpenGLContext> opengl_context)
    : mOpenGLContext { std::move(opengl_context) }
{
}

void yuki::OpenGLGraphicsDevice::setContextCurrent()
{
    mOpenGLContext->setCurrent();
}

std::shared_ptr<yuki::GDTexture> yuki::OpenGLGraphicsDevice::createTexture()
{
    return std::make_shared<OpenGLTexture>();
}

std::shared_ptr<yuki::Shader> yuki::OpenGLGraphicsDevice::createShader()
{
    return std::make_shared<OpenGLShader>();
}

std::shared_ptr<yuki::VertexBuffer> yuki::OpenGLGraphicsDevice::createVertexBuffer()
{
    return std::make_shared<OpenGLVertexBuffer>();
}

void yuki::OpenGLGraphicsDevice::swapFrameBuffers()
{
    mOpenGLContext->swapBuffer();
}

void yuki::OpenGLGraphicsDevice::clearCurrentFrameBuffer()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

std::shared_ptr<yuki::RasterizerState> yuki::OpenGLGraphicsDevice::createRasterizerState()
{
    return std::make_shared<OpenGLRasterizerState>();
}

std::shared_ptr<yuki::BlendState> yuki::OpenGLGraphicsDevice::createBlendState()
{
    return std::make_shared<OpenGLBlendState>();
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

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    YUKI_OPENGL_CHECK();
}

void yuki::OpenGLGraphicsDevice::drawPoints(size_t first, size_t count)
{
    _draw(GL_POINTS, first, count);
}
