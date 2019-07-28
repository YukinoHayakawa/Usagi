﻿#pragma once

#include <Usagi/Core/Component.hpp>
#include <Usagi/Math/Matrix.hpp>

namespace usagi
{
// todo animated transform?
struct TransformComponent : Component
{
private:
    /**
     * \brief Local origin in the parent space.
     */
    Vector3f mPosition = Vector3f::Zero();

    /**
     * \brief An offset added to all vertices.
     */
    Vector3f mOffset = Vector3f::Zero();

    Vector3f mScale = Vector3f::Ones();
    Quaternionf mOrientation = Quaternionf::Identity();

    /**
    * \brief A flag indicating whether mLocalToParent should be re-calculated.
    */
    mutable bool mLTPNeedsUpdate = false;

    mutable Affine3f mLocalToParent = Affine3f::Identity();

    /**
    * \brief Transform from local coordinates to world coordinates.
    * Calculated by TransformSystem using
    * <Parent Element>.localToWorld() * localToParent() if the parent has
    * a TransformComponent. Otherwise identical to localToParent().
    * The calculation cascade towards the root element and is cached if
    * localToParent() is not changed.
    */
    mutable Affine3f mLocalToWorld = Affine3f::Identity();

public:
    Vector3f position() const { return mPosition; }

    void setPosition(const Vector3f &position)
    {
        mPosition = position;
        mLTPNeedsUpdate = true;
    }

    Vector3f offset() const { return mOffset; }

    void setOffset(const Vector3f &offset)
    {
        mOffset = offset;
        mLTPNeedsUpdate = true;
    }

    Vector3f scale() const { return mScale; }

    void setScale(const Vector3f &scale)
    {
        mScale = scale;
        mLTPNeedsUpdate = true;
    }

    Quaternionf orientation() const { return mOrientation; }

    void setOrientation(const Quaternionf &orientation)
    {
        mOrientation = orientation;
        mLTPNeedsUpdate = true;
    }

    void setOrientationPosition(const Affine3f &mat)
    {
        mPosition = mat.translation();
        mOrientation = mat.rotation();
        mLTPNeedsUpdate = true;
    }

    Affine3f localToParent() const
    {
        if(mLTPNeedsUpdate)
        {
            mLocalToParent.setIdentity();
            mLocalToParent.scale(mScale);
            mLocalToParent.rotate(mOrientation);
            mLocalToParent.pretranslate(mPosition + mOffset);
            mLTPNeedsUpdate = false;
        }
        return mLocalToParent;
    }

    TransformComponent * parent = nullptr;

    Affine3f localToWorld() const
    {
        // todo cache
        return parent
            ? parent->localToWorld() * localToParent()
            : localToParent();
    }

    Affine3f worldToLocal() const
    {
        return localToWorld().inverse();
    }

    const std::type_info & baseType() override final
    {
        return typeid(TransformComponent);
    }
};
}
