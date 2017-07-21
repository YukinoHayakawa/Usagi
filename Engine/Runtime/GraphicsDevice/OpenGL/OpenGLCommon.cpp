#include <GL/glew.h>
#include <iostream>

#include "OpenGLCommon.hpp"

void yuki::OpenGLCheckError(const char *file, int line)
{
    const char *errname;
    while(GLenum err = glGetError())
    {
        switch(err)
        {
            case GL_INVALID_ENUM: errname = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE: errname = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: errname = "GL_INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW: errname = "GL_STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW: errname = "GL_STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY: errname = "GL_OUT_OF_MEMORY"; break;
            default: errname = "unknown"; break;
        }
        std::cerr << "[" << file << ":" << line << "] OpenGL call failed: " << errname << std::endl;
    }
}

GLenum yuki::getOpenGLElementType(BufferElementType mChannelDataType)
{
    GLenum type;
    switch(mChannelDataType)
    {
        case BufferElementType::BYTE: type = GL_BYTE; break;
        case BufferElementType::UNSIGNED_BYTE: type = GL_UNSIGNED_BYTE; break;
        case BufferElementType::SHORT: type = GL_SHORT; break;
        case BufferElementType::UNSIGNED_SHORT: type = GL_UNSIGNED_SHORT; break;
        case BufferElementType::INTEGER: type = GL_INT; break;
        case BufferElementType::UNSIGNED_INTEGER: type = GL_UNSIGNED_INT; break;
        case BufferElementType::FLOAT: type = GL_FLOAT; break;
        default: throw std::invalid_argument("invalid BufferElementType value");
    }
    return type;
}
