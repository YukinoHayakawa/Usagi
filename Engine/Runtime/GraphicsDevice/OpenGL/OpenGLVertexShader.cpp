#include "OpenGLVertexShader.hpp"

void yuki::OpenGLVertexShader::useSourceString(std::string source_code)
{
    _useSourceString(source_code);
}

void yuki::OpenGLVertexShader::readFromSourceFile(const std::string &file_path)
{
    _readFromSourceFile(file_path);
}

void yuki::OpenGLVertexShader::compile()
{
    _compile(GL_VERTEX_SHADER);
}
