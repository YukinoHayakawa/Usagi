#pragma once

#include <Usagi/Core/Element.hpp>

namespace usagi
{
struct TransformComponent;

template <typename CameraT, typename CameraControllerT>
class CameraMan : public Element
{
    std::shared_ptr<CameraT> mCamera;
    std::shared_ptr<CameraControllerT> mCameraController;
    TransformComponent *mTransform = nullptr;

public:
    CameraMan(
        Element *parent,
        std::string name,
        std::shared_ptr<CameraT> camera,
        std::shared_ptr<CameraControllerT> camera_controller)
        : Element(parent, std::move(name))
        , mCamera(std::move(camera))
        , mCameraController(std::move(camera_controller))
    {
        mTransform = addComponent<TransformComponent>();
        mCameraController->setTransform(mTransform);
    }

    CameraT * camera() const { return mCamera.get(); }

    CameraControllerT * cameraController() const
    {
        return mCameraController.get();
    }

    TransformComponent * transform() const { return mTransform; }
};
}
