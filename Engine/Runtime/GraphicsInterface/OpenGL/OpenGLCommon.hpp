#pragma once

#include <stdexcept>
#include <GL/glew.h>

#include <Usagi/Engine/Utility/Exception.hpp>

namespace yuki
{

class OpenGLAPIException : public std::runtime_error
{
public:
    OpenGLAPIException(const std::string &_Message, GLenum open_gl_error_code)
        : runtime_error { _Message }
        , mOpenGLErrorCode { open_gl_error_code }
    {
    }

    OpenGLAPIException(const char *_Message, GLenum open_gl_error_code)
        : runtime_error { _Message }
        , mOpenGLErrorCode { open_gl_error_code }
    {
    }

    GLenum getErrorCode() const
    {
        return mOpenGLErrorCode;
    }

protected:
    GLenum mOpenGLErrorCode;
};

YUKI_DECL_RUNTIME_EXCEPTION(OpenGLGeneral);

}

#define YUKI_OPENGL_CHECKED_CALL(...) do { \
    __VA_ARGS__; \
    if(GLenum error = glGetError()) \
        throw OpenGLAPIException(#__VA_ARGS__ " failed!", error); \
} while(0) \
/**/
