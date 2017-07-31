#pragma once

#include <GL/glew.h>

#include <Usagi/Engine/Runtime/GraphicsDevice/GDTexture.hpp>

namespace yuki
{

class OpenGLTexture : public GDTexture
{
    friend class OpenGLPipeline;

    GLuint mTextureName = 0;
    int mWidth = 0, mHeight = 0;

    void _bindToTextureUnit(GLuint unit) const;

    static GLenum _translateInternalFormat(ChannelEnum cenum);
    static GLenum _translateFormat(ChannelEnum cenum);

public:
    OpenGLTexture();
    ~OpenGLTexture() override;

    void setFormat(int width, int height, ChannelEnum format) override;
    void upload(NativeDataType src_channel_type, ChannelEnum src_format, uint8_t src_alignment, const void *data) override;
};

}
