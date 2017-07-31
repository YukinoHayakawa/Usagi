#include <stdexcept>

#include "OpenGLSampler.hpp"
#include "OpenGLCommon.hpp"

void yuki::OpenGLSampler::_bindToTextureUnit(GLuint unit)
{
    glBindSampler(unit, mSamplerName);
}

GLint yuki::OpenGLSampler::_translateFilterType(FilterType filter)
{
    switch(filter)
    {
        case FilterType::NEAREST: return GL_NEAREST;
        case FilterType::LINEAR: return GL_LINEAR;
        default: throw std::logic_error("invalid FilterType value");
    }
}

GLint yuki::OpenGLSampler::_translateTextureWrapping(TextureWrapping wrapping)
{
    switch(wrapping)
    {
        case TextureWrapping::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
        case TextureWrapping::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
        case TextureWrapping::REPEAT: return GL_REPEAT;
        case TextureWrapping::MIRRORED_CLAMP_TO_EDGE: return GL_MIRROR_CLAMP_TO_EDGE;
        default: throw std::logic_error("invalid TextureWrapping value");
    }
}

yuki::OpenGLSampler::OpenGLSampler()
{
    glCreateSamplers(1, &mSamplerName);
}

yuki::OpenGLSampler::~OpenGLSampler()
{
    glDeleteSamplers(1, &mSamplerName);
}

void yuki::OpenGLSampler::setFilter(FilterType minifying, FilterType magnifying)
{
    glSamplerParameteri(mSamplerName, GL_TEXTURE_MIN_FILTER, _translateFilterType(minifying));
    glSamplerParameteri(mSamplerName, GL_TEXTURE_MAG_FILTER, _translateFilterType(magnifying));
    YUKI_OPENGL_CHECK();
}

void yuki::OpenGLSampler::setTextureWrapping(TextureWrapping u, TextureWrapping v)
{
    glSamplerParameteri(mSamplerName, GL_TEXTURE_WRAP_T, _translateTextureWrapping(u));
    glSamplerParameteri(mSamplerName, GL_TEXTURE_WRAP_R, _translateTextureWrapping(v));
    YUKI_OPENGL_CHECK();
}
