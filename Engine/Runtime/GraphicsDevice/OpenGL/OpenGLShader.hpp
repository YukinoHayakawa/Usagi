#pragma once

#include <GL/glew.h>

#include <Usagi/Engine/Runtime/GraphicsDevice/Shader.hpp>

namespace yuki
{

class OpenGLShader : public Shader
{
    GLuint mProgramId = 0, mVertShader = 0, mFragShader = 0, mVAO = 0;
    size_t mLastLayoutSignature = 0;

    void _compileShader(GLenum shaderType, const std::string &source);
    void _deleteVAO();

public:
    OpenGLShader();
    ~OpenGLShader() override;

    OpenGLShader & setVertexShaderSource(const std::string &src) override;
    OpenGLShader & setFragmentShaderSource(const std::string &src) override;
    OpenGLShader & compile() override;
    OpenGLShader & linkInputs(VertexBuffer &vbo) override;
    BindingSentry bind() override;
};

}
