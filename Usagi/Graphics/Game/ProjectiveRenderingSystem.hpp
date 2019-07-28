#pragma once

#include <functional>

#include <Usagi/Math/Matrix.hpp>

#include "RenderableSystem.hpp"

namespace usagi
{
/**
 * \brief Contains common facilities for rendering 3D scenes using projection
 * matrices.
 */
class ProjectiveRenderingSystem : virtual public RenderableSystem
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
