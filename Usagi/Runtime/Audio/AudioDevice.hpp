#pragma once

#include <memory>

#include <Usagi/Utility/Noncopyable.hpp>

#include "AudioStream.hpp"

namespace usagi
{
struct AudioStreamFormat;
class AudioOutputStream;

class AudioDevice : Noncopyable
{
public:
    virtual ~AudioDevice() = default;

    virtual bool isOutputFormatSupported(
        const AudioStreamFormat &format) const = 0;

    virtual std::unique_ptr<AudioOutputStream> createOutputStream(
        const AudioStreamFormat &format,
        AudioOutputCallback callback) = 0;
};
}
