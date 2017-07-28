#include <vector>
#include <cassert>

#include "OpenGLShader.hpp"

#include <Usagi/Engine/Runtime/GraphicsDevice/OpenGL/OpenGLCommon.hpp>
#include <Usagi/Engine/Utility/File.hpp>

void yuki::detail::OpenGLShader::_attachToProgram(GLuint program)
{
    assert(mShader);
    glAttachShader(program, mShader);
}

void yuki::detail::OpenGLShader::_detachFromProgram(GLuint program)
{
    assert(mShader);
    glDetachShader(program, mShader);
}

yuki::detail::OpenGLShader::OpenGLShader()
{

}

yuki::detail::OpenGLShader::~OpenGLShader()
{
    if(mShader) glDeleteShader(mShader);
}

void yuki::detail::OpenGLShader::_useSourceString(std::string source_code)
{
    mSource = source_code;
}

void yuki::detail::OpenGLShader::_readFromSourceFile(const std::string &file_path)
{
    mSource = readFileContent(file_path);
}

void yuki::detail::OpenGLShader::_compile(GLenum shaderType)
{
    if(mShader)
    {
        throw std::logic_error(
            "Shader object already created. Recreation may cause program objects refer to shader object pending deletion."
        );
    }
    mShader = glCreateShader(shaderType);

    const char *src = mSource.c_str();
    glShaderSource(mShader, 1, &src, nullptr);
    printf("Compiling shader\n");
    glCompileShader(mShader);
    // free memory used by the source
    std::string().swap(mSource);

    GLint result;
    int info_log_length;

    glGetShaderiv(mShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(mShader, GL_INFO_LOG_LENGTH, &info_log_length);
    if(info_log_length > 0)
    {
        std::vector<char> info_log(info_log_length + 1);
        glGetShaderInfoLog(mShader, info_log_length, nullptr, &info_log[0]);
        printf("%s\n", &info_log[0]);
    }

    // put at the end so we can see the compiling error log
    YUKI_OPENGL_CHECK();
}
