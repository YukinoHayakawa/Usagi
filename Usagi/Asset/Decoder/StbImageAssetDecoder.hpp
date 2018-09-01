#pragma once

#include <string>

#include "ImageBuffer.hpp"

namespace usagi
{
bool isStbImageSupportedFormat(const std::string &file_extension);

/**
 * \brief Decodes byte stream of JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC file.
 * Requires the stream to be
 */
struct StbImageAssetDecoder
{
    ImageBuffer operator()(std::istream &in) const;
};
}
