#pragma once

#include <cstddef>
#include <memory>

#include <Usagi/Core/Math.hpp>

namespace usagi
{
struct ImageBuffer
{
    using Byte = std::byte;

    std::shared_ptr<Byte> buffer;
    std::size_t buffer_size = 0;
    Vector2u32 image_size = Vector2u32::Zero();
    std::uint32_t channels = 0;

    enum class ChannelFormat
    {
        FLOAT32,
        UINT8,
        // UINT16,
        // UINT32,
    } format = ChannelFormat::UINT8;

    // todo hdr support
    // bool hdr = false;
};
}
