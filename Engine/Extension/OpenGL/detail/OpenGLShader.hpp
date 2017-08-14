#pragma once

#include <string>
#include <GL/glew.h>

#include <Usagi/Engine/Runtime/GraphicsDevice/Shader.hpp>

namespace yuki
{
namespace detail
{

class OpenGLShader : public virtual Shader
{
    GLuint mShader = 0;
    std::string mSource;

protected:
    void _attachToProgram(GLuint program);
    void _detachFromProgram(GLuint program);
    void _compile(GLenum shaderType);

public:
    OpenGLShader();
    ~OpenGLShader();

    void useSourceString(std::string source_code) override;
    void readFromSourceFile(const std::string &file_path) override;
};

}
}
