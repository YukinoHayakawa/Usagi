#pragma once

#include <Usagi/Game/System.hpp>

namespace usagi
{
class InputMapping;

class InputSystem final
    : public System
{
    InputMapping *mInputMapping = nullptr;

public:
    explicit InputSystem(InputMapping *input_mapping);

    void onElementComponentChanged(Element *element) override;

    void update(const Clock &clock) override { }

    const std::type_info & type() override
    {
        return typeid(decltype(*this));
    }
};
}
