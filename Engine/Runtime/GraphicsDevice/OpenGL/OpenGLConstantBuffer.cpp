#include "OpenGLConstantBuffer.hpp"
#include <cassert>
#include "OpenGLCommon.hpp"

// https://www.khronos.org/opengl/wiki/Data_Type_(GLSL)
// Uniform block layout section in:
// https://learnopengl.com/#!Advanced-OpenGL/Advanced-GLSL
yuki::OpenGLConstantBuffer::AttributeAlignment yuki::OpenGLConstantBuffer::_getStd140BaseAlignment(ShaderDataType type)
{
    switch(type)
    {
        // todo: c++ spec says bool size is of 1 byte
        case ShaderDataType::BOOL: return { 4, 4, 1 };
        case ShaderDataType::INTEGER: return { 4, 4, 1 };
        case ShaderDataType::UNSIGNED_INTEGER: return { 4, 4, 1 };
        case ShaderDataType::FLOAT: return { 4, 4, 1 };
        case ShaderDataType::VECTOR2: return { 8, 8, 1 };
        case ShaderDataType::VECTOR3: return { 12, 16, 1 };
        case ShaderDataType::VECTOR4: return { 16, 16, 1 };
        case ShaderDataType::MATRIX3: return { 12, 16, 3 };
        case ShaderDataType::MATRIX4: return { 16, 16, 4 };
        default: throw std::logic_error("invalid ShaderDataType value");
    }
}

void yuki::OpenGLConstantBuffer::setAttributeFormat(const std::vector<yuki::ConstantBuffer::AttributeFormat> &format)
{
    // todo: test the implementation correctness
    mAttributes.clear();
    size_t buf_cursor = 0;
    for(auto &&i : format)
    {
        assert(i.count);
        AttributeAlignment align = _getStd140BaseAlignment(i.type);
        if(i.count != 1)
        {
            align.stride = 16; // align array elements as vec4
        }
        buf_cursor = _calcAlignedOffset(buf_cursor, align.stride);
        size_t elem_size = align.stride * align.count * i.count;
        mAttributes.push_back({
            i.count,
            buf_cursor,
            align
        }); 
        buf_cursor += elem_size;
    }
    _updateSizeInfo(buf_cursor);
    _reallocate();
}

size_t yuki::OpenGLConstantBuffer::setAttributeData(size_t index, const void *data)
{
    auto &attr = mAttributes[index];
    size_t buf_cursor = attr.offset;
    const char *src_cursor = static_cast<const char *>(data);
    // todo: perform ranged mapping
    {
        auto mapping = mapWrite(false);
        for(auto i = 0; i < attr.count; ++i)
        {
            for(auto j = 0; j < attr.batch.count; ++j)
            {
                memcpy(static_cast<char*>(mapping.storage) + buf_cursor, src_cursor, attr.batch.size);
                buf_cursor += attr.batch.stride;
                src_cursor += attr.batch.size;
            }
        }
    }
    YUKI_OPENGL_CHECK();
    return src_cursor - static_cast<const char *>(data);
}
