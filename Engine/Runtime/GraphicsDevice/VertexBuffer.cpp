#include <algorithm>
#include <functional>
#include <string>

#include "VertexBuffer.hpp"

size_t yuki::VertexBuffer::getSize() const
{
    return mSize;
}

yuki::VertexBuffer & yuki::VertexBuffer::setLayout(size_t count, size_t element_size, std::initializer_list<BufferLayout> layout)
{
    mSize = count * element_size;
    mLayout = layout;
    mElementSize = element_size;
	// todo: use string_view
    mSignature = std::hash<std::string>()(std::string(
        reinterpret_cast<const char *>(&mLayout[0]),
        sizeof(BufferLayout) * mLayout.size()
    ));
    _updateLayout();

    return *this;
}

const yuki::BufferLayout * yuki::VertexBuffer::findLayoutEntry(const char *name) const
{
    auto iter = std::find_if(mLayout.begin(), mLayout.end(), [=](auto &entry) {
        return strcmp(entry.name, name) == 0;
    });
    if(iter == mLayout.end()) return nullptr;
    return &*iter;
}

size_t yuki::VertexBuffer::getLayoutSignature() const
{
    return mSignature;
}

size_t yuki::VertexBuffer::getElementSize() const
{
    return mElementSize;
}
