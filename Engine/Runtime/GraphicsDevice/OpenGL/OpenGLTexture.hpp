#pragma once

#include <GL/glew.h>

#include <Usagi/Engine/Runtime/GraphicsDevice/GDTexture.hpp>

namespace yuki
{

class OpenGLTexture : public GDTexture
{
    GLuint mTextureName = 0;
    int mWidth = 0, mHeight = 0;
    uint8_t mNumChannels = 0;
    ShaderDataType mChannelDataType { };

public:
    OpenGLTexture();
    ~OpenGLTexture() override;

    void setFormat(int width, int height, uint8_t num_channels, ShaderDataType channel_data_type) override;
    void upload(const void *data) override;
    BindingSentry bind(size_t slot_id) override;
};

}
