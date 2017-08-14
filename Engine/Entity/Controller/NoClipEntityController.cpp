﻿#include "NoClipEntityController.hpp"
#include <Usagi/Engine/Runtime/HID/Keyboard/KeyCode.hpp>
#include <Usagi/Engine/Component/DynamicComponent.hpp>
#include <Usagi/Engine/Time/Clock.hpp>

void yuki::NoClipEntityController::_resetRotation()
{
    mYaw = 0;
    mPitch = 0;
}

yuki::NoClipEntityController::NoClipEntityController(std::shared_ptr<Mouse> mouse, std::shared_ptr<Keyboard> keyboard)
    // todo remove unused members
    : InteractiveComponent { std::move(mouse), std::move(keyboard) }
{
    _resetRotation();
}

void yuki::NoClipEntityController::tickUpdate(const Clock &clock)
{
    // calculate position
    {
        static const Eigen::Vector3f DIR_VEC[]
        {
            { 0, 0, -1 }, { 0, 0, 1 },
            { -1, 0, 0 }, { 1, 0, 0 },
            { 0, 1, 0 }, { 0, -1, 0 },
        };
        Eigen::Vector3f movement;
        
        for(size_t i = 0; i < static_cast<size_t>(Direction::ENUM_COUNT); ++i)
            if(mMovement[i]) movement += DIR_VEC[i];
        movement.normalize();

        mEntity->addPosition(movement * mMoveSpeed * clock.getElapsedTime());
    }

    // calculate orientation
    {
        auto l2w = mEntity->getLocalToWorldTransform();
        Eigen::Vector3f right = l2w.linear().col(0);

        mEntity->rotate(Eigen::AngleAxisf(mPitch * mRotationSpeed, right));
        mEntity->rotate(Eigen::AngleAxisf(mYaw * mRotationSpeed, Eigen::Vector3f::UnitY()));        

        _resetRotation();
    }
}

void yuki::NoClipEntityController::onMouseMove(const MousePositionEvent &e)
{
    mYaw -= e.cursorPosDelta.x();
    mPitch -= e.cursorPosDelta.y();
}

void yuki::NoClipEntityController::onKeyStateChange(const KeyEvent &e)
{
    switch(e.keyCode)
    {
#define NCEC_KEY_BIND(key, dir) case KeyCode::key: mMovement[static_cast<size_t>(Direction::dir)] = e.pressed; break
        NCEC_KEY_BIND(W, FORWARD);
        NCEC_KEY_BIND(S, BACKWARD);
        NCEC_KEY_BIND(A, LEFT);
        NCEC_KEY_BIND(D, RIGHT);
        NCEC_KEY_BIND(E, UPWARD);
        NCEC_KEY_BIND(Q, DOWNWARD);
#undef NCEC_KEY_BIND
        default: break;
    }
}

void yuki::NoClipEntityController::_postAttachTo()
{
    // enforce the constraints of this entity controller,
    // assuming that the entity will only be modified by us.

    auto l2w = mEntity->getLocalToWorldTransform();

    // build x axis parallel to world x-z plane
    Eigen::Vector3f right = l2w.linear().col(0);
    right.y() = 0;
    right.normalize();

    // build z axis perpendicular to x axis
    Eigen::Vector3f back = right.cross(l2w.linear().col(1));
    back.normalize();

    // build y axis perpendicular to x and z axes
    Eigen::Vector3f up = right.cross(back);

    // update the orientation of the entity
    Eigen::Matrix3f orientation;
    orientation << right, up, back;
    mEntity->setOrientation(Eigen::Quaternionf(orientation));
}
