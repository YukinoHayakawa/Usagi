#include "InputSubsystem.hpp"

#include <Usagi/Core/Element.hpp>

#include "InputComponent.hpp"

usagi::InputSubsystem::InputSubsystem(InputMapping *input_mapping)
    : mInputMapping(input_mapping)
{
}

void usagi::InputSubsystem::onElementComponentChanged(Element *element)
{
    if(const auto comp = element->findComponent<InputComponent>())
    {
        comp->input_mapping = mInputMapping;
    }
}
