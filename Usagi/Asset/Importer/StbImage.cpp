#include "StbImage.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Utility/RAIIHelper.hpp>
#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/Enum/GpuBufferFormat.hpp>
#include <Usagi/Runtime/Graphics/GpuImageCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>

bool usagi::isStbImageSupportedFormat(const char *file_extension)
{
    static auto supported = {
        ".jpg", ".jpeg",
        ".png",
        ".bmp",
        ".tga",
        ".gif",
        ".psd",
        ".hdr",
        ".pic",
        ".ppm", ".pgm",
    };
    return std::find(supported.begin(), supported.end(), file_extension)
        != supported.end();
}

namespace
{
int read(void *user, char *data, const int size)
{
    auto stream = static_cast<std::istream*>(user);
    stream->read(data, size);
    return static_cast<int>(stream->gcount());
}

void skip(void *user, const int size)
{
    const auto stream = static_cast<std::istream*>(user);
    stream->seekg(size, std::ios_base::cur);
}

int eof(void *user)
{
    const auto stream = static_cast<std::istream*>(user);
    return stream->eof();
}

stbi_io_callbacks gCallbacks
{
    &read, &skip, &eof
};
}

std::shared_ptr<usagi::GpuImage> usagi::stbImageImportAsGpuImage(
    GpuDevice *device, std::istream &in)
{
    int x, y, channels;

    stbi_uc *data = nullptr;
    RAIIHelper auto_free {
        [&]() {
            if((data = stbi_load_from_callbacks(
                &gCallbacks, &in, &x, &y, &channels, 0)) == nullptr)
            {
                LOG(error, "Failed to load image: {}", stbi_failure_reason());
                throw std::runtime_error(stbi_failure_reason());
            }
        }, [&]() {
            stbi_image_free(data);
        }
    };

    GpuImageCreateInfo info;
    switch(channels)
    {
        case 1: info.format = GpuBufferFormat::R32_SFLOAT; break;
        case 2: info.format = GpuBufferFormat::R32G32_SFLOAT; break;
        case 3: info.format = GpuBufferFormat::R32G32B32_SFLOAT; break;
        case 4: info.format = GpuBufferFormat::R32G32B32A32_SFLOAT; break;
        default:
            LOG(error, "Invalid channel amount: {}", channels);
            throw std::runtime_error("Invalid image.");
    }
    info.size = { x, y };
    info.usage = GpuImageUsage::SAMPLED;

    auto image = device->createImage(info);
    image->upload(data, x * y * channels * sizeof(stbi_uc));
    return std::move(image);
}
