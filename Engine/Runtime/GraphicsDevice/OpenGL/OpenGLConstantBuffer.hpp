#pragma once

#include <vector>

#include <Usagi/Engine/Runtime/GraphicsDevice/ConstantBuffer.hpp>
#include "detail/OpenGLBuffer.hpp"

namespace yuki
{

/**
 * \brief The shader should use std140 layout for uniform blocks.
 */
class OpenGLConstantBuffer : public ConstantBuffer, detail::OpenGLBuffer
{
    friend class OpenGLPipeline;

    /**
     * \brief Control the copying behavior from host to
     * buffer of one element. Attributes in the buffer are padded
     * differently than in host. For example, a MATRIX3 may have the
     * layout as:
     * float float float char[4]
     * float float float char[4]
     * float float float char[4]
     * while the source data is tightly packed.
     * In this case, batch_size = 3*sizeof(float),
     * batch_padding = 4, batch_count = 3.
     * The array elements are also aligned to 16 bytes.
     */
    struct AttributeAlignment
    {
        uint8_t size; // host bytes copied in each batch
        uint8_t stride; // batch_size + buffer bytes skipped after each batch, same as alignment
        uint8_t count; // number of batches of each array element
    };

    struct AttributeInfo
    {
        size_t count; // number of array elements
        size_t offset; // aligned offset

        AttributeAlignment batch;
    };

    std::vector<AttributeInfo> mAttributes;

    /**
     * \brief The base alignment of type is the space it takes in byte,
     * including the paddings after it.
     * \param type 
     * \return 
     */
    static AttributeAlignment _getStd140BaseAlignment(ShaderDataType type);

public:
    void setAttributeFormat(std::initializer_list<AttributeFormat> format) override;
    size_t setAttributeData(size_t index, const void *data) override;
};

}
