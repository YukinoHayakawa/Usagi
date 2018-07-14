#pragma once

#include "ComponentEvent.hpp"

namespace usagi
{
class ComponentAddedEvent : public ComponentEvent
{
public:
	ComponentAddedEvent(const std::type_info &type, Component *component)
		: ComponentEvent { type, component }
	{
	}
};
}
