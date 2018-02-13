#pragma once

namespace yuki::graphics
{
enum class ResourceState
{
    // the memory for the resource is not allocated, or the content
    // of it is non initialized.
    UNINITIALIZED,
    // the resource is ready to use.
    IDLE,
    // the content of the resource is being uploading to the device.
    STREAMING,
    // the resource is being actively referenced by the device.
    WORKING,
};
}
