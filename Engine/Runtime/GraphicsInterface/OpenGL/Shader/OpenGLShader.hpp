#pragma once

#include <GL/glew.h>
#include <string>
#include <sstream>

namespace yuki
{

class OpenGLShader
{
    GLuint mShaderId = 0;

    void _init(GLenum shaderType, const std::string &source);

public:
    OpenGLShader(GLenum shaderType, const std::string &source);

    template <typename Stream>
    OpenGLShader(GLenum shaderType, Stream &&source)
    {
        std::ostringstream in;
        std::string line;
        while(getline(source, line))
        {
            in << line << '\n';
        }
        _init(shaderType, in.str());
    }

    ~OpenGLShader();

    GLuint getId() const { return mShaderId; }
};

}
