#pragma once

#include "MemoryHandle.hpp"

namespace usagi::runtime::allocators
{
template <typename Allocator>
void validate_before_resolve(const Allocator *, const MemoryHandle handle)
{
    USAGI_CHECK_THROW(
        LogicException,
        !handle.is_valid() || handle.signature == Allocator::SIGNATURE,
        "memory handle not allocator by this kind of allocator");
}

enum class AllocatorType : std::uint8_t
{
    Bitmap     = 0b1000,
    Linear     = 0b0100,
    Buddy      = 0b0010,
    TLSF       = 0b1100,
    Hive       = 0b1010,
    Reserved_1 = 0b0110,
    Reserved_2 = 0b1110,
    _64BitFlag = 0b0001,
};
} // namespace usagi::runtime::allocators

template <>
struct usagi::EnableBitMaskOperators<usagi::runtime::allocators::AllocatorType>
    : std::true_type
{
};

namespace usagi::runtime::allocators
{
constexpr std::uint8_t allocator_signature(
    const AllocatorType base_type, const bool is_64_bit = false)
{
    if(is_64_bit)
    {
        return static_cast<std::uint8_t>(base_type | AllocatorType::_64BitFlag);
    }
    return static_cast<std::uint8_t>(base_type);
}
} // namespace usagi::runtime::allocators
