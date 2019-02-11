#include "ModelViewCameraController.hpp"

#include <Usagi/Transform/TransformComponent.hpp>

usagi::ModelViewCameraController::ModelViewCameraController(
    Vector3f look_at_target,
    const float distance)
    : mLookAtTarget(std::move(look_at_target))
    , mDistance(distance)
{
}

// todo accumulate the changes
void usagi::ModelViewCameraController::rotate(Vector2f rel)
{
    rel.y() *= mYPositivity;
    mSpherical += rel * mSensitivity;
    // maintain numerical accuracy
    mSpherical.x() = std::fmod(mSpherical.x(), 360.f);
    // prevent gimbal lock
    mSpherical.y() = std::clamp(mSpherical.y(), -89.f, 89.f);

    /* not tested
    const auto theta = degreesToRadians(mSpherical.y());
    const auto phi = degreesToRadians(mSpherical.x());
    const auto sin_theta = std::sin(theta);
    const auto x = sin_theta * std::cos(phi);
    const auto y = sin_theta * std::sin(phi);
    const auto z = std::cos(theta);

    mTransform.local_to_parent = lookAt(Vector3f { x, y, z } * mDistance);
    */

    auto transform = Affine3f::Identity();
    transform.rotate(AngleAxisf(
        degreesToRadians(mSpherical.x()), Vector3f::UnitZ()));
    transform.rotate(AngleAxisf(
        degreesToRadians(mSpherical.y()), Vector3f::UnitX()));
    transform.translate(Vector3f::UnitY() * -mDistance);
    mTransform->setOrientationPosition(transform);
}
