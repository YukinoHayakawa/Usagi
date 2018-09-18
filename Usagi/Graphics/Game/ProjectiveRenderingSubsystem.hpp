#pragma once

#include <functional>

#include <Usagi/Core/Math.hpp>

#include "RenderableSubsystem.hpp"

namespace usagi
{
/**
 * \brief Contains common facilities for rendering 3D scenes using projection
 * matrices.
 */
class ProjectiveRenderingSubsystem : virtual public RenderableSubsystem
{
protected:
    /**
    * \brief The matrix for transforming from world coordinates to
    * clip space.
    */
    using WorldToNdcFunc = std::function<Projective3f()>;
    WorldToNdcFunc mWorldToNdcFunc;

public:
    void setWorldToNdcFunc(WorldToNdcFunc func)
    {
        mWorldToNdcFunc = std::move(func);
    }
};
}
