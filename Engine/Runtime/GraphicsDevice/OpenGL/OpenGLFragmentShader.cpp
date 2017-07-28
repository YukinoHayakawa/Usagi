#include "OpenGLFragmentShader.hpp"

void yuki::OpenGLFragmentShader::useSourceString(std::string source_code)
{
    _useSourceString(source_code);
}

void yuki::OpenGLFragmentShader::readFromSourceFile(const std::string &file_path)
{
    _readFromSourceFile(file_path);
}

void yuki::OpenGLFragmentShader::compile()
{
    _compile(GL_FRAGMENT_SHADER);
}
