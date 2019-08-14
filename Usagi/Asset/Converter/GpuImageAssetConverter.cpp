#include "GpuImageAssetConverter.hpp"

#include <Usagi/Asset/Decoder/ImageBuffer.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Runtime/Graphics/Enum/GpuBufferFormat.hpp>
#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>
#include <Usagi/Runtime/Graphics/GpuImageCreateInfo.hpp>

std::shared_ptr<usagi::GpuImage>
usagi::GpuImageAssetConverter::operator()(
    AssetLoadingContext *ctx,
    const ImageBuffer &buffer,
    GpuDevice *device) const
{
    if(buffer.format != ImageBuffer::ChannelFormat::UINT8)
    {
        LOG(error, "Only uint8 images are implemented yet.");
        USAGI_THROW(std::runtime_error("Unimplemented image format."));
    }

    GpuImageCreateInfo info;
    switch(buffer.channels)
    {
        case 1: info.format = GpuBufferFormat::R8_UNORM; break;
        case 2: info.format = GpuBufferFormat::R8G8_UNORM; break;
        case 3: info.format = GpuBufferFormat::R8G8B8_UNORM; break;
        case 4: info.format = GpuBufferFormat::R8G8B8A8_UNORM; break;
        default:
            LOG(error, "Invalid channel amount: {}", buffer.channels);
            USAGI_THROW(std::runtime_error("Invalid image."));
    }
    info.size = buffer.image_size;
    info.usage = GpuImageUsage::SAMPLED;

    auto image = device->createImage(info);
    image->upload(buffer.buffer.get(), buffer.buffer_size);
    return image;
}
