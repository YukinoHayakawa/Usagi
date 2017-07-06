#include <vector>

#include "OpenGLShader.hpp"
#include "../OpenGLCommon.hpp"

yuki::OpenGLShader::OpenGLShader(GLenum shaderType, const std::string &source)
{
    _init(shaderType, source);
}

void yuki::OpenGLShader::_init(GLenum shaderType, const std::string &source)
{
    YUKI_OPENGL_CHECKED_CALL(mShaderId = glCreateShader(shaderType));
    const char *src = source.c_str();
    glShaderSource(mShaderId, 1, &src, nullptr);
    printf("Compiling shader\n");
    YUKI_OPENGL_CHECKED_CALL(glCompileShader(mShaderId));

    GLint result;
    int info_log_length;

    glGetShaderiv(mShaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(mShaderId, GL_INFO_LOG_LENGTH, &info_log_length);
    if(info_log_length > 0)
    {
        std::vector<char> FragmentShaderErrorMessage(info_log_length + 1);
        glGetShaderInfoLog(mShaderId, info_log_length, nullptr, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }
}

yuki::OpenGLShader::~OpenGLShader()
{
    glDeleteShader(mShaderId);
}
