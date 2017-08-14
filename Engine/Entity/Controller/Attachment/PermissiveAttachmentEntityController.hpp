#pragma once

#include <Usagi/Engine/Entity/Controller/EntityController.hpp>

namespace yuki
{

/**
 * \brief A intermediate controller concept which accept any attaching
 * entity.
 */
class PermissiveAttachmentEntityController : public EntityController
{
protected:
    std::shared_ptr<class DynamicComponent> mEntity;

public:
    void attachTo(std::shared_ptr<DynamicComponent> entity) override;

private:
    virtual void _postAttachTo() { }
};

}
