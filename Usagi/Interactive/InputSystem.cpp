#include "InputSystem.hpp"

#include <Usagi/Core/Element.hpp>

#include "InputComponent.hpp"

usagi::InputSystem::InputSystem(InputMapping *input_mapping)
    : mInputMapping(input_mapping)
{
}

void usagi::InputSystem::onElementComponentChanged(Element *element)
{
    if(const auto comp = element->findComponent<InputComponent>())
    {
        comp->input_mapping = mInputMapping;
    }
}
