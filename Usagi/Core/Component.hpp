#pragma once

#include <typeinfo>

#include <Usagi/Utility/Noncopyable.hpp>

namespace usagi
{
/**
 * \brief Piece of data of an entity.
 * todo: to maximize the utility of data-oriented design, cache-friendliness and data locality should be considered. the components should be stored linearly. component database & id based entity?
 */
class Component : Noncopyable
{
public:
    virtual ~Component() = default;

    /**
     * \brief Identify the type of component. A component must have a base type
     * and a subsystem may create derived component from it. This methods
     * returns the base type of the component. One entity can only have one
     * component of a specific base type such as GraphicsComponent.
     * This method can be implemented in the base type as final.
     * \return
     */
    virtual const std::type_info & baseType() = 0;
};
}
