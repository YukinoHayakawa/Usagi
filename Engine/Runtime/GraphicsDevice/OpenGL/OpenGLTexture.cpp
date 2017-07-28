#include <stdexcept>

#include "OpenGLTexture.hpp"
#include "OpenGLCommon.hpp"

yuki::OpenGLTexture::OpenGLTexture()
{
    glGenTextures(1, &mTextureName);
}

yuki::OpenGLTexture::~OpenGLTexture()
{
    glDeleteTextures(1, &mTextureName);
}

void yuki::OpenGLTexture::setFormat(int width, int height, uint8_t num_channels, ShaderDataType channel_data_type)
{
    mWidth = width;
    mHeight = height;
    mNumChannels = num_channels;
    mChannelDataType = channel_data_type;
}


void yuki::OpenGLTexture::upload(const void *data)
{
    glBindTexture(GL_TEXTURE_2D, mTextureName);

    // declare the source pixels as tightly packed
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLenum type = OpenGLTranslateNativeDataType(mChannelDataType);

    GLint internalFormat;
    switch(mNumChannels)
    {
        case 1: internalFormat = GL_RED; break;
        case 2: internalFormat = GL_RG; break;
        case 3: internalFormat = GL_RGB; break;
        case 4: internalFormat = GL_RGBA; break;
        default: throw std::invalid_argument("invalid pixel channel amount");
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, internalFormat, type, data);
    YUKI_OPENGL_CHECK();

    glBindTexture(GL_TEXTURE_2D, 0);
}

yuki::BindingSentry yuki::OpenGLTexture::bind(size_t slot_id)
{
    glActiveTexture(GL_TEXTURE0 + slot_id);
    glBindTexture(GL_TEXTURE_2D, mTextureName);
    glActiveTexture(GL_TEXTURE0);
    YUKI_OPENGL_CHECK();

    return { [=]()
    {
        glActiveTexture(GL_TEXTURE0 + slot_id);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
    } };
}
