#pragma once

namespace usagi
{
struct TransformComponent;

class CameraController
{
protected:
    TransformComponent *mTransform = nullptr;

public:
    TransformComponent * transform() const { return mTransform; }
    void setTransform(TransformComponent *transform) { mTransform = transform; }
};
}
