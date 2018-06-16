#pragma once

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
};
}
