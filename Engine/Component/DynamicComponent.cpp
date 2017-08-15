#include "DynamicComponent.hpp"

void yuki::DynamicComponent::lookAt(const Eigen::Vector3f &position, const Eigen::Vector3f &target, const Eigen::Vector3f &up)
{
    // todo: geometry::makeCoordinateSystem method
    Eigen::Matrix3f localToWorld;
    localToWorld.col(2) = (position - target).normalized(); // z
    localToWorld.col(0) = up.cross(localToWorld.col(2)).normalized(); // x
    localToWorld.col(1) = localToWorld.col(2).cross(localToWorld.col(0)); // y
    setOrientation(localToWorld);
    setPosition(position);
}

Eigen::Affine3f yuki::DynamicComponent::getLocalToWorldTransform() const
{
    if(_isMatrixDirty())
    {
        mOrientation.normalize();
        mLocalToWorld = Eigen::Translation3f(mPosition) * mOrientation.toRotationMatrix();
        _cleanMatrix();
    }
    return mLocalToWorld;
}
