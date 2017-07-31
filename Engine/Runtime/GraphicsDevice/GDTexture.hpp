#pragma once

#include "NativeDataType.hpp"

namespace yuki
{

/**
 * \brief
 */
class GDTexture
{
public:
    virtual ~GDTexture() = default;

    enum class ChannelEnum
    {
        R, RG, RGB, RGBA
    };

    /**
     * \brief Specify the format of device side texture storage.
     * \param width 
     * \param height 
     * \param format Number of channels and order. An RGBA order is always used.
     */
    virtual void setFormat(int width, int height, ChannelEnum format) = 0;

    /**
     * \brief 
     * \param src_channel_type The data type of each color component of source data.
     * \param src_format Number of channels of the source data. If this is more than that of the target storage,
     * only the required number of channels will be read from each pixel starting from the red channel.
     * \param src_alignment Used to derive the pitch of row of the source data.
     * Must be 1, 2, 4, or 8. If data is tightly packed, use 1.
     * \param data The source data of texture content.
     */
    virtual void upload(NativeDataType src_channel_type, ChannelEnum src_format, uint8_t src_alignment, const void *data) = 0;
};

}
