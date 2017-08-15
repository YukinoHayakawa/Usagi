#pragma once

#include <Usagi/Engine/Component/DynamicComponent.hpp>

namespace yuki
{

/**
 * \brief 
 * projection matrix
 * ray generation
 * 
 * derive from entity - animation: last & next frame transform 
 */
class Camera : public DynamicComponent
{
public:
    /**
     * \brief Get the transformation from camera local space to
     * normalized device coordinates, which is right-handed and
     * having the bounding volume from (-1,-1,-1) to (1,1,1).
     * \return 
     */
    virtual Eigen::Projective3f getProjectionMatrix() = 0;
};

}
