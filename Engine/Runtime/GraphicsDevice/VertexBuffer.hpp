#pragma once

#include "GDBuffer.hpp"

namespace yuki
{

class VertexBuffer : public GDBuffer
{
protected:
    size_t mSize = 0, mElementSize = 0;

    void _updateFormat(size_t count, size_t element_size)
    {
        mSize = count * element_size;
        mElementSize = element_size;
    }

public:
    virtual ~VertexBuffer() = default;

    virtual void initStorage(size_t count, size_t element_size) = 0;

    size_t getElementSize() const { return mElementSize; }
};

}
