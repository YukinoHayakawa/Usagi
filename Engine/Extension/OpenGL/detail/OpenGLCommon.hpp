#pragma once

#include <GL/glew.h>

#include <Usagi/Engine/Runtime/GraphicsDevice/NativeDataType.hpp>

namespace yuki
{

void OpenGLCheckError(const char *file, int line);
GLenum OpenGLTranslateNativeDataType(NativeDataType type);

}

#define YUKI_OPENGL_CHECK() yuki::OpenGLCheckError(__FILE__, __LINE__)
