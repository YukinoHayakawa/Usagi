#pragma once

namespace yuki
{

class GDSampler
{
public:
    virtual ~GDSampler() = default;

    enum class FilterType
    {
        NEAREST, LINEAR,
    };

    virtual void setFilter(FilterType minifying, FilterType magnifying) = 0;

    enum class TextureWrapping
    {
        CLAMP_TO_EDGE, MIRRORED_CLAMP_TO_EDGE,
        MIRRORED_REPEAT, REPEAT,
    };

    virtual void setTextureWrapping(TextureWrapping u, TextureWrapping v) = 0;
};

}
