#pragma once

namespace yuki
{

class GraphicsBuffer
{
protected:
    size_t mSize = 0, mAlignment = 0;

public:
    GraphicsBuffer() = default;
    virtual ~GraphicsBuffer() = default;

    void * map();
    virtual void * map(size_t offset, size_t size) = 0;
    virtual void unmap() = 0;

    size_t getSize() const { return mSize; }
    size_t getAlignment() const { return mAlignment; }
};

}
