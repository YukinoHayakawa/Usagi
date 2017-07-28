#include "OpenGLVertexBuffer.hpp"

void yuki::OpenGLVertexBuffer::initStorage(size_t count, size_t element_size)
{
    _updateFormat(count, element_size);
    _reallocate();
}
