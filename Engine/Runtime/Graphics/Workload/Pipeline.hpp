#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki::graphics
{

/**
 * \brief Immutable opaque API-specific representation of the states of
 * programmable and fixed-function stages of a graphics pipeline.
 * An instance is created by passing a PipelineCreateInfo to the constructor
 * of an implementation.
 */
class Pipeline : Noncopyable
{
public:
    virtual ~Pipeline() = default;
};

}
