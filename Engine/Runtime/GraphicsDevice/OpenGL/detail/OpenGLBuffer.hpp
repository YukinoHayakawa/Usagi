#pragma once

#include <GL/glew.h>

#include <Usagi/Engine/Runtime/GraphicsDevice/GDBuffer.hpp>

namespace yuki
{
namespace detail
{

class OpenGLBuffer : public virtual GDBuffer
{
    GLuint mBufferName = 0;

protected:
    GLuint _getBufferName() const { return mBufferName; }
    void _reallocate();

    void streamFromHostBuffer(const void *data, size_t length) override;
    MemoryMappingSentry mapWrite(bool orphan_old) override;

public:
    OpenGLBuffer();
    ~OpenGLBuffer() override;
};

}
}
