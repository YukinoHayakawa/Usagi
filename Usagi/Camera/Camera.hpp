#pragma once

#include <Usagi/Math/Matrix.hpp>
#include <Usagi/Geometry/Ray.hpp>

namespace usagi
{
struct CameraSample;

// todo add TransformComponent?
class Camera
{
public:
    virtual ~Camera() = default;

    /**
     * \brief Get the transformation from camera local space to
     * right-handed normalized device coordinates. The value ranges of NDC
     * are defined as following:
     *
     * x: [-1, 1]
     * y: [-1, 1]
     * z: [0, 1] (depth, near plane to far plane)
     *
     * Note that in order to achieve perspective projection, the position
     * transformed using this matrix should be component-wise-divided by its w
     * value.
     *
     * The construction of the matrix can be decomposed into two steps:
     *
     * 1. Transform from camera local space to raster space (x, y are
     * framebuffer coordinates, z is [0, 1] depth range).
     * The matrix produced by this step can be used to generate camera ray.
     * 2. Transform from framebuffer to NDC. This step is actually independent
     * of camera type.
     *
     * todo camera only have to impl step 2. (non-pinhole camera have to override ray generation method)
     *
     * Camera coordinates:
     *      z (Up)
     *      |  y (Shooting direction)
     *      | /
     *      |/
     *      -------- x
     *
     * NDC & Screen coordinates:
     *         z
     *        /
     *       /
     *      -------- x
     *      |
     *      |
     *      y
     * \return
     */
    virtual Projective3f localToNDC() const = 0;

    // cast ray in camera local space
    virtual Ray generateRay(const CameraSample &sample) const = 0;
};
}
