#pragma once

#include <Usagi/Core/PredefinedElement.hpp>

namespace usagi
{
struct TransformComponent;

/**
 * \brief A camera controlling element for attaching to other objects.
 * \tparam CameraT
 * \tparam CameraControllerT
 */
template <typename CameraT, typename CameraControllerT>
class CameraMan : public PredefinedElement<TransformComponent>
{
    std::shared_ptr<CameraT> mCamera;
    std::shared_ptr<CameraControllerT> mCameraController;

public:
    CameraMan(
        Element *parent,
        std::string name,
        std::shared_ptr<CameraT> camera,
        std::shared_ptr<CameraControllerT> camera_controller)
        : PredefinedElement(parent, std::move(name))
        , mCamera(std::move(camera))
        , mCameraController(std::move(camera_controller))
    {
        mCameraController->setTransform(comp<TransformComponent>());
    }

    CameraT * camera() const { return mCamera.get(); }

    CameraControllerT * cameraController() const
    {
        return mCameraController.get();
    }
};
}
