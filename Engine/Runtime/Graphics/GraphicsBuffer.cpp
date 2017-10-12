#include "GraphicsBuffer.hpp"

using namespace yuki;

void * yuki::GraphicsBuffer::map()
{
    return map(0, mSize);
}
