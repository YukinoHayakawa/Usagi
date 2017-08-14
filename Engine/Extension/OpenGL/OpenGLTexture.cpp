#include <stdexcept>

#include "OpenGLTexture.hpp"
#include "detail/OpenGLCommon.hpp"

void yuki::OpenGLTexture::_bindToTextureUnit(GLuint unit) const
{
    glBindTextureUnit(unit, mTextureName);
}

GLenum yuki::OpenGLTexture::_translateInternalFormat(ChannelEnum cenum)
{
    switch(cenum)
    {
        case ChannelEnum::R: return GL_R32F;
        case ChannelEnum::RG: return GL_RG32F;
        case ChannelEnum::RGB: return GL_RGB32F;
        case ChannelEnum::RGBA: return GL_RGBA32F;
        default: throw std::logic_error("invalid ChannelEnum value");
    }
}

GLenum yuki::OpenGLTexture::_translateFormat(ChannelEnum cenum)
{
    switch(cenum)
    {
        case ChannelEnum::R: return GL_RED;
        case ChannelEnum::RG: return GL_RG;
        case ChannelEnum::RGB: return GL_RGB;
        case ChannelEnum::RGBA: return GL_RGBA;
        default: throw std::logic_error("invalid ChannelEnum value");
    }
}

yuki::OpenGLTexture::OpenGLTexture()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &mTextureName);
}

yuki::OpenGLTexture::~OpenGLTexture()
{
    glDeleteTextures(1, &mTextureName);
}

void yuki::OpenGLTexture::setFormat(int width, int height, ChannelEnum format)
{
    mWidth = width; mHeight = height;
    // note that internal format must be sized
    glTextureStorage2D(mTextureName, 1, _translateInternalFormat(format), width, height);
    YUKI_OPENGL_CHECK();
}

void yuki::OpenGLTexture::upload(NativeDataType src_channel_type, ChannelEnum src_format, uint8_t src_alignment, const void *data)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, src_alignment);
    glTextureSubImage2D(
        mTextureName,
        0,
        0, 0,
        mWidth, mHeight,
        _translateFormat(src_format),
        OpenGLTranslateNativeDataType(src_channel_type),
        data
    );
    YUKI_OPENGL_CHECK();
}
