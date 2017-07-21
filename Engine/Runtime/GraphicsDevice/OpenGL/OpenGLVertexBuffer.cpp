#include "OpenGLCommon.hpp"

#include "OpenGLVertexBuffer.hpp"

yuki::OpenGLVertexBuffer::OpenGLVertexBuffer()
{
    glGenBuffers(1, &mBufferName);
}

yuki::OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    if(mBufferName) glDeleteBuffers(1, &mBufferName);
    mBufferName = 0;
}

void yuki::OpenGLVertexBuffer::streamUpdate(const void *data, size_t length)
{
    auto sentry = bind();
    glBufferData(GL_ARRAY_BUFFER, mSize, nullptr, GL_STREAM_DRAW); // orphan the old buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, length, data);
    YUKI_OPENGL_CHECK();
}

yuki::VertexBuffer::MemoryMappingSentry yuki::OpenGLVertexBuffer::mapStorage()
{
    glBindBuffer(GL_ARRAY_BUFFER, mBufferName);
    auto mem = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    YUKI_OPENGL_CHECK();

    return { [this]()
    {
        glBindBuffer(GL_ARRAY_BUFFER, mBufferName);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }, mem };
}

void yuki::OpenGLVertexBuffer::reallocate()
{
    auto sentry = bind();
    glBufferData(GL_ARRAY_BUFFER, mSize, nullptr, GL_STREAM_DRAW);
    YUKI_OPENGL_CHECK();
}

yuki::BindingSentry yuki::OpenGLVertexBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, mBufferName);
    YUKI_OPENGL_CHECK();

    return { []()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    } };
}

void yuki::OpenGLVertexBuffer::_updateLayout()
{
}
