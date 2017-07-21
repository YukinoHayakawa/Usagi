#pragma once

#include <initializer_list>
#include <vector>
#include <functional>

#include <Usagi/Engine/Utility/BindingSentry.hpp>
#include "BufferElementType.hpp"

namespace yuki
{


/**
* \brief Describe the data layout of vertex buffer.
*/
struct BufferLayout
{
    const char          *name;
    BufferElementType   element_type;
    uint8_t             num_elements;
    bool                normalized;
    size_t              offset;
};

/**
 * \brief Provide input data for shaders.
 * 
 * The instance of implementation class shall be created from a GraphicsDevice
 * instance. The user must define the layout of data before providing the buffer
 * to a shader, when the shader binds shader inputs to locations in the buffer,
 * using the names defined in the layout.
 * 
 * todo: create a template derived class to use reflection on its template parameter to generate layout information
 * todo: support HLSL semantic names
 * todo: support multiple vertex buffers
 */
class VertexBuffer
{
protected:
    size_t mSize = 0, mElementSize = 0;
    std::vector<BufferLayout> mLayout;

    class MemoryMappingSentry : public BindingSentry
    {
    public:
        void *const storage;

        MemoryMappingSentry(void *storage)
            : BindingSentry { []() {} }
            , storage { storage }
        {
        }

        MemoryMappingSentry(std::function<void()> unmap_func, void *storage)
            : BindingSentry { std::move(unmap_func) }
            , storage { storage }
        {
        }
    };

public:
    virtual ~VertexBuffer() = default;

    size_t getSize() const;

    VertexBuffer & setLayout(size_t count, size_t element_size, std::initializer_list<BufferLayout> layout);
    const BufferLayout * findLayoutEntry(const char *name) const;
    size_t getLayoutSignature() const;
    size_t getElementSize() const;

    /**
     * \brief Dispose the old buffer storage and allocate a new one of the same size,
     * then upload data into the buffer.
     * \param data The pointer to the start of the data being uploaded.
     * \param length The length of data. An std::runtime_exception is thrown if greater
     * than getSize().
     */
    virtual void streamUpdate(const void *data, size_t length) = 0;
    virtual MemoryMappingSentry mapStorage() = 0;
    virtual void reallocate() = 0;

    virtual BindingSentry bind() = 0;

private:
    virtual void _updateLayout() = 0;

    size_t mSignature = 0;
};

}
