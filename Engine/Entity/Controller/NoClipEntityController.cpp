#include <Usagi/Engine/Runtime/HID/Keyboard/KeyCode.hpp>
#include <Usagi/Engine/Component/DynamicComponent.hpp>
#include <Usagi/Engine/Time/Clock.hpp>
#include <Usagi/Engine/Runtime/HID/Mouse/Mouse.hpp>

#include "NoClipEntityController.hpp"

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
    mMouse->setImmersiveMode(true);
}

void yuki::NoClipEntityController::tickUpdate(const Clock &clock)
{
    if(!mMouse->isImmersiveMode()) return;

    // calculate position
    {
        static const Eigen::Vector3f DIR_VEC[]
        {
            { 0, 0, -1 }, // forward
            { 0, 0, 1 }, // backward
            { -1, 0, 0 }, // left
            { 1, 0, 0 }, // right
            { 0, 1, 0 }, // up
            { 0, -1, 0 }, // down
        };
        Eigen::Vector3f movement = Eigen::Vector3f::Zero();

        // pan along local axes but locked in world x-z plane
        for(size_t i = 0; i < static_cast<size_t>(Direction::UPWARD); ++i)
            if(mMovement[i]) movement += DIR_VEC[i];
        if(!movement.isZero())
        {
            // todo make a method of DynamicComponent::moveInPlane()?
            movement = mEntity->getLocalToWorldTransform().linear() * movement;
            movement.y() = 0; // lock in x-z plane
            movement.normalize();
            mEntity->move(movement * mMoveSpeed * clock.getElapsedTime());
            movement.setZero();
        }

        // rise and drop in world coordinates
        for(size_t i = static_cast<size_t>(Direction::UPWARD); i < static_cast<size_t>(Direction::ENUM_COUNT); ++i)
            if(mMovement[i]) movement += DIR_VEC[i];
        if(!movement.isZero())
        {
            movement.normalize();
            mEntity->move(movement * mMoveSpeed * 0.25f * clock.getElapsedTime());
            movement.setZero();
        }
    }

    // calculate orientation
    {
        auto l2w = mEntity->getLocalToWorldTransform();
        Eigen::Vector3f right = l2w.linear().col(0);

        // todo: lock view angle
        mEntity->rotate(Eigen::AngleAxisf(mPitch * mRotationSpeed, right));
        mEntity->rotate(Eigen::AngleAxisf(mYaw * mRotationSpeed, Eigen::Vector3f::UnitY()));        

        _resetRotation();
    }
}

void yuki::NoClipEntityController::onMouseMove(const MousePositionEvent &e)
{
    if(!mMouse->isImmersiveMode()) return;

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
        NCEC_KEY_BIND(SPACE, UPWARD);
        NCEC_KEY_BIND(LEFT_SHIFT, DOWNWARD);
#undef NCEC_KEY_BIND
        case KeyCode::LEFT_CONTROL:
        {
            if(!e.repeated) mMouse->setImmersiveMode(!e.pressed);
            break;
        }
        default: break;
    }
}

// todo: geometry::makeCoordinateSystem method
// todo: not working correctly after several frames
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
    mEntity->setOrientation(orientation);
}
