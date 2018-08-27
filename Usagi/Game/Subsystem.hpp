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
     * \brief Returns whether the entity will be processed by the subsystem.
     * \param element
     */
    virtual bool processable(Element *element) = 0;

    /**
    * \brief Check whether the entity has relevant components. If it has,
    * record it inside the subsystem. If it doesn't have, remove its
    * record from the subsystem record if it exists. This is called by
    * the first event listener on component add/remove events at the root
    * entity by the Game object.
    * \param element
    */
    virtual void updateRegistry(Element *element) = 0;
};
}
