#pragma once

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
    Projective3f mWorldToNDC = Projective3f::Identity();

public:
    /**
     * \brief Must be called before calling update() and render().
     * \param mat
     */
    void setWorldToNDC(const Projective3f &mat)
    {
        mWorldToNDC = mat;
    }
};
}
