#pragma once

namespace yuki
{

class Clock;

/**
 * \brief Use as a mixin class for objects depending on time steps,
 * such as renderers and animated objects.
 */
class TimeVariantComponent
{
public:
    virtual ~TimeVariantComponent() = default;

    virtual void tickUpdate(const Clock &clock) = 0;
};

}
