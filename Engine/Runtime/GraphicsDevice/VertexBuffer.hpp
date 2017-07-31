#pragma once

#include "GDBuffer.hpp"

namespace yuki
{

class VertexBuffer : public virtual GDBuffer
{
protected:
    size_t mElementSize = 0;

    void _updateFormat(size_t count, size_t element_size)
    {
        _updateSizeInfo(count * element_size);
        mElementSize = element_size;
    }

public:
    virtual ~VertexBuffer() = default;

    virtual void initStorage(size_t count, size_t element_size) = 0;
    size_t getElementSize() const { return mElementSize; }

    // expose as public methods
    using GDBuffer::streamFromHostBuffer;
    using GDBuffer::mapWrite;
};

}
