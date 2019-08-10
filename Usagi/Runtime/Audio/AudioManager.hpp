#pragma once

#include <memory>
#include <vector>

#include <Usagi/Utility/Noncopyable.hpp>

#include "AudioDeviceProperties.hpp"

namespace usagi
{
class AudioDevice;

class AudioDeviceAcquisitionAgent
{
    class AudioManager *mManager = nullptr;
    int mDeviceIndex;
    AudioDeviceProperties mProperties;

public:
    AudioDeviceAcquisitionAgent(
        AudioManager *manager,
        int device_index,
        AudioDeviceProperties properties);

    std::shared_ptr<AudioDevice> acquireDevice() const;

    const AudioDeviceProperties & properties() const
    {
        return mProperties;
    }

    int deviceIndex() const
    {
        return mDeviceIndex;
    }
};

class AudioManager : Noncopyable
{
public:
    using AcquisitionAgent = AudioDeviceAcquisitionAgent;

protected:
    std::vector<AcquisitionAgent> mDevices;

public:
    virtual ~AudioManager() = default;

    virtual const std::vector<AcquisitionAgent> & enumerateDevices() = 0;
    virtual AcquisitionAgent defaultOutputDevice() = 0;
    // virtual const AcquisitionAgent & defaultInputDevice() = 0;

    static constexpr int DEFAULT_OUTPUT_DEVICE = -1;
    static constexpr int DEFAULT_INPUT_DEVICE = -2;

    virtual std::shared_ptr<AudioDevice> createDevice(int index) = 0;
};
}
