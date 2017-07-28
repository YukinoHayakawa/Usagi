#pragma once

#include <Usagi/Engine/Runtime/GraphicsDevice/Shader.hpp>
#include "detail/OpenGLShader.hpp"

namespace yuki
{

class OpenGLFragmentShader : public FragmentShader, detail::OpenGLShader
{
    friend class OpenGLPipeline;

public:
    void useSourceString(std::string source_code) override;
    void readFromSourceFile(const std::string &file_path) override;
    void compile() override;
};

}
