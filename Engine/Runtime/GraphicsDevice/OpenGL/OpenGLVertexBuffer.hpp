#pragma once

#include <Usagi/Engine/Runtime/GraphicsDevice/VertexBuffer.hpp>
#include "detail/OpenGLBuffer.hpp"

namespace yuki
{

class OpenGLVertexBuffer : public VertexBuffer, public detail::OpenGLBuffer
{
    friend class OpenGLPipeline;

public:
    void initStorage(size_t count, size_t element_size) override;
};

}
