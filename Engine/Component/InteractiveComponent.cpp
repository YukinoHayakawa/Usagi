#include "InteractiveComponent.hpp"

yuki::InteractiveComponent::InteractiveComponent(std::shared_ptr<Mouse> mouse, std::shared_ptr<Keyboard> keyboard)
    : mMouse { std::move(mouse) }
    , mKeyboard { std::move(keyboard) }
{
}
