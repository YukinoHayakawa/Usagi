#include <GL/glew.h>
#include <iostream>

#include "OpenGLCommon.hpp"

void yuki::OpenGLCheckError(const char *file, int line)
{
    // todo: possible implicit synchronization
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

GLenum yuki::OpenGLTranslateNativeDataType(NativeDataType type)
{
    switch(type)
    {
        case NativeDataType::BYTE: return GL_BYTE;
        case NativeDataType::UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
        case NativeDataType::SHORT: return GL_SHORT;
        case NativeDataType::UNSIGNED_SHORT: return GL_UNSIGNED_SHORT;
        case NativeDataType::INTEGER: return GL_INT;
        case NativeDataType::UNSIGNED_INTEGER: return GL_UNSIGNED_INT;
        case NativeDataType::FLOAT: return GL_FLOAT;
        case NativeDataType::BOOL: return GL_BOOL;
        default: throw std::invalid_argument("invalid NativeDataType value");
    }
}
