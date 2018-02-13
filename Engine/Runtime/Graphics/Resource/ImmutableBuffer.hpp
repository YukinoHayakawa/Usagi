#pragma once

#include "Buffer.hpp"

namespace yuki::graphics
{
/**
 * \brief A buffer whose content cannot be changed once initialized.
 * Therefore it does not need any synchornization of access.
 */
class ImmutableBuffer : public Buffer
{
public:
    virtual void initialize(void *data) = 0;
};
}
