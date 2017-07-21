#pragma once

#include <GL/glew.h>

#include <Usagi/Engine/Runtime/GraphicsDevice/VertexBuffer.hpp>

namespace yuki
{

class OpenGLVertexBuffer : public VertexBuffer
{
public:
    void streamUpdate(const void *data, size_t length) override;
    MemoryMappingSentry mapStorage() override;
    void reallocate() override;
    BindingSentry bind() override;

private:
    GLuint mBufferName = 0;

    void _updateLayout() override;
};

}
