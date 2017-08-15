#pragma once

#include "Attachment/PermissiveAttachmentEntityController.hpp"
#include <Usagi/Engine/Component/InteractiveComponent.hpp>
#include <Usagi/Engine/Geometry/Direction.hpp>

namespace yuki
{

/**
 * \brief An entity controller primarily for debugging and cheating,
 * which changes the position of attaching entity with predefined key
 * presses and rotates it with mouse movement.
 * 
 * The key table is defined as following:
 * 
 * W - move forward
 * S - move backward
 * A - move left
 * D - move right
 * SPACE - move upward
 * LEFT_SHIFT - move downward
 * 
 * where the coordinate system is defined by the following rules:
 * Y axis is aligned with the world coordinate system with the same
 * direction;
 * X is obtained by setting the Y compoenent of the same axis of
 * the entity before normalization;
 * Z is the cross product of X and Y axes.
 * 
 * This controller performs incremental operations on the entity it
 * attaches to, without resetting the initial state of the entity
 * during binding. During its usage, only translational and rotational
 * components of the transformation of the entity will be preserved.
 * 
 * todo: this is an extremely simple impl of camera controller. improve the algorithm using books or other resources.
 */
class NoClipEntityController
    : public PermissiveAttachmentEntityController
    , public InteractiveComponent
{
protected:
    bool mMovement[static_cast<size_t>(Direction::ENUM_COUNT)] = { false };
    float mMoveSpeed = 0.4f; // m/s
    float mRotationSpeed = 0.1f; // deg/pixel
    float mYaw; // rotation around world Y axis
    float mPitch; // rotation around local X axis

private:
    void _resetRotation();

public:
    NoClipEntityController(std::shared_ptr<Mouse> mouse, std::shared_ptr<Keyboard> keyboard);

    void tickUpdate(const Clock &clock) override;
    void onMouseMove(const MousePositionEvent &e) override;
    void onKeyStateChange(const KeyEvent &e) override;

private:
    void _postAttachTo() override;
};

}
