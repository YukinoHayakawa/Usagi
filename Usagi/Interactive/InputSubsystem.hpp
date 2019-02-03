#pragma once

#include <Usagi/Game/Subsystem.hpp>

namespace usagi
{
class InputMapping;

class InputSubsystem : public Subsystem
{
    InputMapping *mInputMapping = nullptr;

public:
    explicit InputSubsystem(InputMapping *input_mapping);

    void onElementComponentChanged(Element *element) override;

    void update(const Clock &clock) override { }
};
}
