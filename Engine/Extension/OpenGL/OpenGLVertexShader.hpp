#pragma once

#include <Usagi/Engine/Runtime/GraphicsDevice/Shader.hpp>
#include "detail/OpenGLShader.hpp"

namespace yuki
{

class OpenGLVertexShader : public VertexShader, detail::OpenGLShader
{
    friend class OpenGLPipeline;

public:
    void compile() override;
};

}
