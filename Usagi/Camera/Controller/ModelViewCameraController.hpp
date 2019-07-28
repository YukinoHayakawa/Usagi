#pragma once

#include <Usagi/Math/Matrix.hpp>

#include "CameraController.hpp"

namespace usagi
{
/**
 * \brief Camera always look at a target from a fixed distance.
 */
class ModelViewCameraController : public CameraController
{
    Vector3f mLookAtTarget = Vector3f::Zero();
    // the radius of the spherical coordinate system
    float mDistance = 10.f;

    float mSensitivity = 0.2f;
    float mYPositivity = 1.f; // set -1.f to reverse Y axis

    // by default look at the target from the front
    // <Azimuth (cw angle from X), Inclination (angle from Z)>, in degrees
    Vector2f mSpherical = Vector2f::Zero();

public:
    ModelViewCameraController(Vector3f look_at_target, float distance);

    /**
     * \brief
     * \param rel relative <dAzimuth, dInclination> angles, in degrees
     */
    void rotate(Vector2f rel);
};
}
