#pragma once

#include <GL/glew.h>

#include <Usagi/Engine/Runtime/GraphicsDevice/BufferElementType.hpp>

namespace yuki
{

void OpenGLCheckError(const char *file, int line);
GLenum getOpenGLElementType(BufferElementType mChannelDataType);

}

#define YUKI_OPENGL_CHECK() yuki::OpenGLCheckError(__FILE__, __LINE__)
