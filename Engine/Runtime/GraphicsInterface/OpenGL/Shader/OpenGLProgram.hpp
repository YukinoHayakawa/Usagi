#pragma once

#include <GL/glew.h>

namespace yuki
{

class OpenGLShader;

class OpenGLProgram
{
    GLuint mProgramId;

public:
    OpenGLProgram()
    {
        mProgramId = glCreateProgram();
    }

    void attachShader(const OpenGLShader &shader);
    // todo: auto detach shaders
    void link();

    GLuint getId() const { return mProgramId; }
};

}
