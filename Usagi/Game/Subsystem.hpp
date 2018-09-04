#pragma once

#include <Usagi/Core/Time.hpp>
#include <Usagi/Utility/Noncopyable.hpp>

namespace usagi
{
class Element;

class Subsystem : Noncopyable
{
public:
    virtual ~Subsystem() = default;

    /**
     * \brief Update the state of subsystem based on provided time step.
     * \param dt
     */
    virtual void update(const TimeDuration &dt) = 0;

    /**
     * \brief Called every time when a component is added or removed.
     * The subsystem can inspect the element to decide to record it or
     * remove it from the record.
     * \param element
     */
    virtual void onElementComponentChanged(Element *element) = 0;
};
}
