#pragma once

#include <GL/glew.h>

#include <Usagi/Engine/Runtime/GraphicsDevice/GDSampler.hpp>

namespace yuki
{

class OpenGLSampler : public GDSampler
{
    friend class OpenGLPipeline;

    GLuint mSamplerName = 0;

    void _bindToTextureUnit(GLuint unit);

    static GLint _translateFilterType(FilterType filter);
    static GLint _translateTextureWrapping(TextureWrapping wrapping);

public:
    OpenGLSampler();
    ~OpenGLSampler() override;

    void setFilter(FilterType minifying, FilterType magnifying) override;
    void setTextureWrapping(TextureWrapping u, TextureWrapping v) override;
};

}
