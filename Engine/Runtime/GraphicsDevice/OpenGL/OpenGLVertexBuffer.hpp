#pragma once

#include <GL/glew.h>

#include <Usagi/Engine/Runtime/GraphicsDevice/VertexBuffer.hpp>

namespace yuki
{

class OpenGLVertexBuffer : public VertexBuffer
{
    friend class OpenGLPipeline;

public:
    OpenGLVertexBuffer();
    ~OpenGLVertexBuffer() override;
    void _reallocate();

    void streamFromHostBuffer(const void *data, size_t length) override;
    MemoryMappingSentry mapWrite(bool orphan_old) override;
    void initStorage(size_t count, size_t element_size) override;

private:
    GLuint mBufferName = 0;
    GLuint _getBufferName() const { return mBufferName; }

};

}
