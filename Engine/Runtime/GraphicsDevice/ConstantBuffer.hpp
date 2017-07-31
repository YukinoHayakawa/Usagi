#pragma once

#include "GDBuffer.hpp"
#include "ShaderDataType.hpp"

namespace yuki
{

/**
 * \brief 
 * todo: add method for setting array elements
 */
class ConstantBuffer : public virtual GDBuffer
{
public:
    struct AttributeFormat
    {
        ShaderDataType type;
        size_t count;
    };

    /**
     * \brief Specify the data types of the attributes in the same order as in the buffer.
     * This information is then used to calculate the actual layout with paddings.
     * \param format 
     */
    virtual void setAttributeFormat(const std::vector<AttributeFormat> &format) = 0;
    /**
     * \brief 
     * \param index 
     * \param data 
     * \return Total bytes read from data.
     */
    virtual size_t setAttributeData(size_t index, const void *data) = 0;

protected:
    /**
     * \brief Calculate the aligned address of element.
     * \param cursor_pos Next available position of bytes.
     * \param alignment The alignment requirement of the type.
     * \return 
     */
    static size_t _calcAlignedOffset(int64_t cursor_pos, int64_t alignment)
    {
        // return cursor_pos + (alignment - cursor_pos % alignment) % alignment;
        // Wolfram|Alpha said this is simpler.
        return -cursor_pos % alignment + cursor_pos;
    }
};

}
