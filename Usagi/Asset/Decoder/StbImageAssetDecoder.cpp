#include "StbImageAssetDecoder.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <Usagi/Core/Logging.hpp>

bool usagi::isStbImageSupportedFormat(const std::string &file_extension)
{
    static std::initializer_list<std::string> supported = {
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

usagi::ImageBuffer usagi::StbImageAssetDecoder::operator()(
    std::istream &in) const
{
    int x, y, channels;
    auto *data = stbi_load_from_callbacks(
        &gCallbacks, &in, &x, &y, &channels, 0);
    if(data == nullptr)
    {
        LOG(error, "Failed to load image: {}", stbi_failure_reason());
        throw std::runtime_error(stbi_failure_reason());
    }

    ImageBuffer img;

    img.buffer = {
        reinterpret_cast<ImageBuffer::Byte*>(data), stbi_image_free
    };
    img.buffer_size = x * y * channels * sizeof(std::uint8_t);
    img.image_size = { x, y };
    img.channels = channels;
    img.format = ImageBuffer::ChannelFormat::UINT8;
    // img.hdr = false;

    return img;
}
