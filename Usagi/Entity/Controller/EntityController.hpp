#pragma once

#include <memory>

#include <Usagi/Engine/Component//TimeVariantComponent.hpp>

namespace yuki
{

/**
 * \brief Manipulate an entity on each time step.
 */
class EntityController : public TimeVariantComponent
{
public:
    /**
     * \brief Establish the control of <entity>. The old attachment is
     * dropped if any exists. The attaching entity is updated on each calling to tickUpdate().
     * \param entity 
     */
    virtual void attachTo(std::shared_ptr<class DynamicComponent> entity) = 0;
};

}
