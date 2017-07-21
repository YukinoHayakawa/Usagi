#pragma once

#include <memory>
#include <GL/glew.h>

#include "../GraphicsDevice.hpp"

namespace yuki
{

class OpenGLContext;

class OpenGLGraphicsDevice : public GraphicsDevice
{
    std::shared_ptr<OpenGLContext> mOpenGLContext;

    void _draw(GLenum mode, size_t first, size_t count);

public:
    OpenGLGraphicsDevice(std::shared_ptr<OpenGLContext> opengl_context);
    
    void setContextCurrent() override;
    std::shared_ptr<GDTexture> createTexture() override;
    std::shared_ptr<Shader> createShader() override;

    std::shared_ptr<VertexBuffer> createVertexBuffer() override;
    void swapFrameBuffers() override;
    void clearCurrentFrameBuffer() override;

    std::shared_ptr<RasterizerState> createRasterizerState() override;
    std::shared_ptr<BlendState> createBlendState() override;
    void setScissorRect(int x, int y, int width, int height) override;

    void drawPoints(size_t first, size_t count) override;
    void drawLines(size_t first, size_t count) override;
    void drawTriangles(size_t first, size_t count) override;
};

}
