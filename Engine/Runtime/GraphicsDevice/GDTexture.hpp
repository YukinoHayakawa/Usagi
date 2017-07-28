#pragma once

#include <functional>

#include "ShaderDataType.hpp"
#include <Usagi/Engine/Utility/BindingSentry.hpp>

namespace yuki
{

/**
 * \brief
 * todo: alignment
 */
class GDTexture
{
public:
    virtual ~GDTexture() = default;

    /**
     * \brief Specify the pixel format of the texture. Invocation to this function does not
     * imply any calling to the graphics API, and only after uploadData() is called with a
     * success the buffer may be bound with a shader input slot.
     * \param width 
     * \param height 
     * \param num_channels 
     * \param channel_data_type 
     */
    virtual void setFormat(int width, int height, uint8_t num_channels, ShaderDataType channel_data_type) = 0;

    /**
     * \brief Copy data to the texture buffer. Must be called after setFormat(), then the
     * texture may be used by shaders if the operation succeeds.
     * The source pixel components must be ordered in RGBA, and are tightly packed without any
     * padding after any pixel or row.
     * todo: deal with alignment
     * \param data A pointer to the beginning of the texture data, whose size must be at least
     * width * height * sizeof([pixel]).
     */
    virtual void upload(const void *data) = 0;

    virtual BindingSentry bind(size_t slot_id) = 0;
};

}
