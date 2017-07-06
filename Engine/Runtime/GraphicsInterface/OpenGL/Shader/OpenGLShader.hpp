#pragma once

#include <GL/glew.h>
#include <string>

namespace yuki
{

class OpenGLShader
{
    GLuint mShaderId = 0;

public:
    OpenGLShader(GLenum shaderType, const std::string &source);
    ~OpenGLShader();

    GLuint getId() const { return mShaderId; }
};

}
