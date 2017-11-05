#pragma once

namespace yuki::graphics
{

struct SamplerCreateInfo
{
    enum class Filter
    {
        NEAREST,
        LINEAR,
    } magnify_filter, minify_filter;

    enum class AddressMode
    {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
    } address_mode_u, address_mode_v;
};

}
