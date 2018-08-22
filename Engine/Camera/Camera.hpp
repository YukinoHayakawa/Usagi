#pragma once

#include <Usagi/Engine/Core/Math.hpp>

namespace usagi
{
class Camera
{
public:
    virtual ~Camera() = default;

    /**
     * \brief Get the transformation from camera local space to
     * right-handed normalized device coordinates, whose bounding volume
     * is (-1, -1, -1) to (1, 1, 1).
     *
     * Camera coordinates:
     *      z (Up)
     *      |  y (Shooting direction)
     *      | /
     *      |/
     *      -------- x
     *
     * NDC coordinates:
     *         z
     *        /
     *       /
     *      -------- x
     *      |
     *      |
     *      y
     * \return
     */
    virtual Projective3f projectionMatrix() const = 0;
};
}
