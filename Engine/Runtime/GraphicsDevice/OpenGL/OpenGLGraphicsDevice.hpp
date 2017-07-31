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

    void _draw(GLenum mode, size_t first, size_t num_indices);

public:
    OpenGLGraphicsDevice(std::shared_ptr<OpenGLContext> opengl_context);
    
    void setContextCurrent() override;

    std::shared_ptr<GDTexture> createTexture() override;
    std::shared_ptr<GDSampler> createSampler() override;

    std::shared_ptr<ConstantBuffer> createConstantBuffer() override;
    std::shared_ptr<VertexBuffer> createVertexBuffer() override;

    std::shared_ptr<VertexShader> createVertexShader() override;
    std::shared_ptr<FragmentShader> createFragmentShader() override;
    
    std::shared_ptr<GDPipeline> createPipeline() override;

    void swapFrameBuffers() override;
    void clearCurrentFrameBuffer() override;

    void setScissorRect(int x, int y, int width, int height) override;

    void drawPoints(size_t first, size_t num_indices) override;
    void drawLines(size_t first, size_t num_indices) override;
    void drawTriangles(size_t first, size_t num_indices) override;
};

}
