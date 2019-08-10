#pragma once

#include <Usagi/Utility/Noncopyable.hpp>

#include "AudioStreamFormat.hpp"
#include "AudioStream.hpp"

namespace usagi
{
class AudioOutputStream : Noncopyable
{
protected:
    AudioStreamFormat mFormat;
    AudioOutputCallback mCallback;

public:
    AudioOutputStream(AudioStreamFormat format, AudioOutputCallback callback);
    virtual ~AudioOutputStream() = default;

    const AudioStreamFormat & format() const
    {
        return mFormat;
    }

    virtual void start() = 0;
    virtual void stop() = 0;
};
}
