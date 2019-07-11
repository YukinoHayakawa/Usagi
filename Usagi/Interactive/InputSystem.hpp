#pragma once

#include <Usagi/Game/System.hpp>

namespace usagi
{
class InputMapping;

class InputSystem : public System
{
    InputMapping *mInputMapping = nullptr;

public:
    explicit InputSystem(InputMapping *input_mapping);

    void onElementComponentChanged(Element *element) override;

    void update(const Clock &clock) override { }
};
}
