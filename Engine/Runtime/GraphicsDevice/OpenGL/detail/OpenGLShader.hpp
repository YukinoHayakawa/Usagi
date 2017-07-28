#pragma once

#include <string>
#include <GL/glew.h>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{
namespace detail
{

class OpenGLShader : public Noncopyable
{
    GLuint mShader = 0;
    std::string mSource;

protected:
    void _attachToProgram(GLuint program);
    void _detachFromProgram(GLuint program);

public:
    OpenGLShader();
    ~OpenGLShader();

    void _useSourceString(std::string source_code);
    void _readFromSourceFile(const std::string &file_path);
    void _compile(GLenum shaderType);
};

}
}
