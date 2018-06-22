#pragma once

#include <typeinfo>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{
/**
 * \brief Piece of data of an entity.
 */
class Component : Noncopyable
{
public:
    virtual ~Component() = default;

    /**
     * \brief Identify the type of component. A component may have a base type
     * and a subsystem may create derived component from it. This methods
     * returns the base type of the component.
     * This method can be implemented in the base type such as GraphicsComponent
     * as final.
     * \return 
     */
    virtual const std::type_info & getBaseTypeInfo() = 0;
};
}
