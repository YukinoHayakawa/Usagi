#pragma once

#include <cstddef>
#include <string_view>

namespace usagi
{
enum class MemoryViewMutability
{
    MUTABLE,
    READONLY
};

template <MemoryViewMutability Mutability>
class MemoryViewBase
{
    const void *mBaseAddress = nullptr;
    std::size_t mSize = 0;

    constexpr static bool IsMutable()
    {
        return Mutability == MemoryViewMutability::MUTABLE;
    }

    constexpr static bool IsReadonly()
    {
        return Mutability == MemoryViewMutability::READONLY;
    }

    template <MemoryViewMutability OtherMutability>
    friend class MemoryViewBase;

public:
    MemoryViewBase() = default;

    MemoryViewBase(const void *base_address, const std::size_t size)
        // If the view is constructed with a pointer to const address,
        // subsequent modification to the pointed region cannot be allowed.
        requires (IsReadonly())
        : mBaseAddress(base_address)
        , mSize(size)
    {
    }

    MemoryViewBase(void *base_address, const std::size_t size)
        : mBaseAddress(base_address)
        , mSize(size)
    {
    }

    explicit MemoryViewBase(const std::string_view str)
        // Preserve the semantic of string_view as a readonly view of a string.
        requires (IsReadonly())
        : mBaseAddress(str.data())
        , mSize(str.size())
    {
    }

    template <MemoryViewMutability OtherMutability>
    MemoryViewBase(const MemoryViewBase<OtherMutability> &other)
        requires (IsReadonly() || other.IsMutable())
        : mBaseAddress { other.mBaseAddress }
        , mSize { other.mSize }
    {
    }

    template <MemoryViewMutability OtherMutability>
    MemoryViewBase & operator=(const MemoryViewBase<OtherMutability> &other)
        requires (IsReadonly() || other.IsMutable())
    {
        if(this == &other)
            return *this;
        mBaseAddress = other.mBaseAddress;
        mSize = other.mSize;
        return *this;
    }

    const void * base_address() const { return mBaseAddress; }

    void * mutable_base_address() const
        requires (IsMutable())
    {
        return const_cast<void *>(mBaseAddress);
    }

    std::size_t size() const { return mSize; }

    operator bool() const
    {
        return mBaseAddress && mSize;
    }

    std::string_view to_string_view() const
    {
        return { static_cast<const char *>(mBaseAddress), mSize };
    }

    const char * as_chars() const
    {
        return static_cast<const char *>(mBaseAddress);
    }

    template <typename ElemT>
    const ElemT * as() const
    {
        return static_cast<const ElemT *>(mBaseAddress);
    }

    template <typename ElemT>
    ElemT * as_mutable() const requires (IsMutable())
    {
        return static_cast<ElemT *>(mutable_base_address());
    }

    bool contains(void *ptr) const
    {
        return ptr >= begin() && ptr < end();
    }

    const void * begin() const { return base_address(); }
    const void * end() const { return as_chars() + size(); }
};

using MemoryView = MemoryViewBase<MemoryViewMutability::MUTABLE>;
using ReadonlyMemoryView = MemoryViewBase<MemoryViewMutability::READONLY>;

struct MemoryRange
{
    std::size_t offset = 0;
    std::size_t size = 0;
};
}
