#pragma once

#include <Usagi/Core/Component.hpp>

namespace usagi
{
class InputMapping;

/**
 * \brief Provide access to input system.
 */
struct InputComponent : Component
{
    InputMapping *input_mapping = nullptr;

    const std::type_info & baseType() override
    {
        return typeid(InputComponent);
    }
};
}
