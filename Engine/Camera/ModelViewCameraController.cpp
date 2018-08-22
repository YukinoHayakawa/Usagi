#include "ModelViewCameraController.hpp"

#include <Usagi/Engine/Transform/TransformComponent.hpp>

usagi::ModelViewCameraController::ModelViewCameraController(
    TransformComponent &transform,
    Vector3f look_at_target,
    const float distance)
    : mTransform(transform)
    , mLookAtTarget(std::move(look_at_target))
    , mDistance(distance)
{
}

// todo cumulate the changes
void usagi::ModelViewCameraController::rotate(Vector2f rel)
{
    rel.y() *= mYPositivity;
    mSpherical += rel * mSensitivity;
    // prevent gimbal lock
    mSpherical.x() = std::clamp(mSpherical.y(), -85.f, 85.f);
    // maintain numerical accuracy
    mSpherical.y() = std::fmod(mSpherical.y(), 360.f);

    // convert to rotation
    auto &transform = mTransform.local_to_parent;
    transform = Affine3f::Identity();
    transform.translate(Vector3f { 0.f, -mDistance, 0.f });
    transform.rotate(AngleAxisf(
        degreesToRadians(mSpherical.y()), Vector3f::UnitZ()));
    transform.rotate(AngleAxisf(
        degreesToRadians(mSpherical.x()), Vector3f::UnitX()));

    mTransform.needs_update = true;
}
