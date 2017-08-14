#include <cassert>

#include "../detail/OpenGLCommon.hpp"
#include "OpenGLBuffer.hpp"

void yuki::detail::OpenGLBuffer::_reallocate()
{
    assert(mSize);
    glNamedBufferData(mBufferName, mSize, nullptr, GL_STREAM_DRAW);
}

void yuki::detail::OpenGLBuffer::streamFromHostBuffer(const void *data, size_t length)
{
    _reallocate();
    glNamedBufferSubData(mBufferName, 0, length, data);
    YUKI_OPENGL_CHECK();
}

yuki::GDBuffer::MemoryMappingSentry yuki::detail::OpenGLBuffer::mapWrite(bool orphan_old)
{
    if(orphan_old) _reallocate();

    auto mem = glMapNamedBuffer(mBufferName, GL_WRITE_ONLY);
    YUKI_OPENGL_CHECK();

    return { [this]() {
        glUnmapNamedBuffer(mBufferName);
    }, mem };
}

yuki::detail::OpenGLBuffer::OpenGLBuffer()
{
    glCreateBuffers(1, &mBufferName);
}

yuki::detail::OpenGLBuffer::~OpenGLBuffer()
{
    if(mBufferName) glDeleteBuffers(1, &mBufferName);
}
