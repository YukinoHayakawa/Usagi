#include "PermissiveAttachmentEntityController.hpp"

void yuki::PermissiveAttachmentEntityController::attachTo(std::shared_ptr<DynamicComponent> entity)
{
    mEntity = std::move(entity);
}
