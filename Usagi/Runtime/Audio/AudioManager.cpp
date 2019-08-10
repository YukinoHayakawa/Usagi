#include "AudioManager.hpp"

namespace usagi
{
AudioDeviceAcquisitionAgent::AudioDeviceAcquisitionAgent(
    AudioManager *manager,
    int device_index,
    AudioDeviceProperties properties)
    : mManager(manager)
    , mDeviceIndex(device_index)
    , mProperties(std::move(properties))
{
}

std::shared_ptr<AudioDevice> AudioDeviceAcquisitionAgent::acquireDevice() const
{
    return mManager->acquireDevice(mDeviceIndex);
}
}
