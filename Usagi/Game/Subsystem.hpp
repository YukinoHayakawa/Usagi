#pragma once

#include <Usagi/Utility/Noncopyable.hpp>

namespace usagi
{
class Clock;
class Element;

class Subsystem : Noncopyable
{
public:
    virtual ~Subsystem() = default;

    /**
     * \brief Update the state of subsystem based on provided time step.
     * \param clock
     */
    virtual void update(const Clock &clock) = 0;

    /**
     * \brief Called every time when a component is added or removed.
     * The subsystem can inspect the element to decide to record it or
     * remove it from the record.
     * \param element
     */
    virtual void onElementComponentChanged(Element *element) = 0;
};
}
