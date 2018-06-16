#pragma once

#include <chrono>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{
class Entity;

class Subsystem : Noncopyable
{
public:
    virtual ~Subsystem() = default;

    /**
     * \brief Update the state of subsystem based on provided time step.
     * \param dt 
     */
    virtual void update(const std::chrono::seconds &dt) = 0;

    /**
     * \brief Determines whether the entity is interested by the subsystem.
     * \param entity 
     */
    virtual bool canProcess(Entity *entity) = 0;

    /**
    * \brief Check whether the entity has relevant components. If it has,
    * record it inside the subsystem. If it doesn't have, remove its
    * record from the subsystem record if it exists. This is called by
    * the first event listener on component add/remove events at the root
    * entity by the Game object.
    * \param entity
    */
    virtual void updateRegistry(Entity *entity) = 0;
};
}
