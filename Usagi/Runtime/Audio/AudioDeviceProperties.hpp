#pragma once

#include <string>

#include "AudioStreamFormat.hpp"

namespace usagi
{
struct AudioDeviceProperties
{
    std::string name;
    std::string api_name;

    double default_sample_rate = 0;

    struct
    {
        std::uint8_t max_channels = 0;
        bool is_default_device = false;
        // todo: fill this vector
        std::vector<AudioStreamFormat> supported_formats;
    } in, out;

    // todo: fill this vector
    std::vector<AudioStreamFormat> full_duplex_formats;
};
}
