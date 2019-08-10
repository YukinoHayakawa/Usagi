#include "AudioOutputStream.hpp"

namespace usagi
{
AudioOutputStream::AudioOutputStream(
    AudioStreamFormat format,
    AudioOutputCallback callback)
    : mFormat(std::move(format))
    , mCallback(std::move(callback))
{
}
}
