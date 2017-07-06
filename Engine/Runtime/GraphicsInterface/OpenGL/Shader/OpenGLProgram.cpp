#include <vector>

#include "OpenGLShader.hpp"
#include "OpenGLProgram.hpp"

void yuki::OpenGLProgram::attachShader(const OpenGLShader &shader)
{
    glAttachShader(mProgramId, shader.getId());
}

void yuki::OpenGLProgram::link()
{
    printf("Linking shaders\n");
    glLinkProgram(mProgramId);

    GLint result;
    int info_log_length;

    // Check the program
    glGetProgramiv(mProgramId, GL_LINK_STATUS, &result);
    glGetProgramiv(mProgramId, GL_INFO_LOG_LENGTH, &info_log_length);
    if(info_log_length > 0)
    {
        std::vector<char> ProgramErrorMessage(info_log_length + 1);
        glGetProgramInfoLog(mProgramId, info_log_length, nullptr, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }
}
