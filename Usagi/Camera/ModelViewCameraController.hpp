#pragma once

#include <Usagi/Core/Math.hpp>

namespace usagi
{
struct TransformComponent;

/**
 * \brief Camera always look at a target from a fixed distance.
 */
class ModelViewCameraController
{
    TransformComponent &mTransform;

    Vector3f mLookAtTarget = Vector3f::Zero();
    // the radius of the spherical coordinate system
    float mDistance = 10.f;

    float mSensitivity = 0.2f;
    float mYPositivity = 1.f; // set -1.f to reverse Y axis

    // by default look at the target from the front
    // <Azimuth (cw angle from X), Inclination (angle from Z)>, in degrees
    Vector2f mSpherical = Vector2f::Zero();

public:
    ModelViewCameraController(
        TransformComponent &transform,
        Vector3f look_at_target,
        float distance);

    /**
     * \brief
     * \param rel <dAzimuth, dInclination>, in degrees
     */
    void rotate(Vector2f rel);
};
}
