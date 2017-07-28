#include "OpenGLCommon.hpp"

#include "OpenGLVertexBuffer.hpp"

yuki::OpenGLVertexBuffer::OpenGLVertexBuffer()
{
    glCreateBuffers(1, &mBufferName);
}

yuki::OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    if(mBufferName) glDeleteBuffers(1, &mBufferName);
}

void yuki::OpenGLVertexBuffer::_reallocate()
{
    glNamedBufferData(mBufferName, mSize, nullptr, GL_STREAM_DRAW);
}

void yuki::OpenGLVertexBuffer::streamFromHostBuffer(const void *data, size_t length)
{
    _reallocate();
    glNamedBufferSubData(mBufferName, 0, length, data);
    YUKI_OPENGL_CHECK();
}

yuki::GDBuffer::MemoryMappingSentry yuki::OpenGLVertexBuffer::mapWrite(bool orphan_old)
{
    if(orphan_old) _reallocate();

    auto mem = glMapNamedBuffer(mBufferName, GL_WRITE_ONLY);
    YUKI_OPENGL_CHECK();

    return { [this]() {
        glUnmapNamedBuffer(mBufferName);
    }, mem };
}

void yuki::OpenGLVertexBuffer::initStorage(size_t count, size_t element_size)
{
    _updateFormat(count, element_size);
    _reallocate();
}
