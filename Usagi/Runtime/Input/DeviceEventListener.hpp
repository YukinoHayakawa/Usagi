#pragma once

namespace usagi
{
class Device;

struct DeviceEvent
{
    Device *device;
};

class DeviceEventListener
{
public:
    virtual ~DeviceEventListener() = default;

    /**
     * \brief Sent when the device is unplugged.
     * \param e 
     */
    virtual void onDeviceRemoved(const DeviceEvent &e) = 0;
};
}
