#pragma once

namespace yuki::graphics
{

/**
 * \brief Opaque base for GPU synchronization objects.
 */
class Waitable
{
public:
    virtual ~Waitable() = default;
};

}
