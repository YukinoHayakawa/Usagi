#pragma once

#include "ComponentEvent.hpp"

namespace usagi
{
class PreComponentRemovalEvent : public ComponentEvent
{
public:
	PreComponentRemovalEvent(const std::type_info &type, Component *component)
		: ComponentEvent { type, component }
	{
	}
};
}
