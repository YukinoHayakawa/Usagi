#pragma once

#include "ComponentEvent.hpp"

namespace usagi
{
class PostComponentRemovalEvent : public ComponentEvent
{
public:
    explicit PostComponentRemovalEvent(const std::type_info &type)
		: ComponentEvent { type, nullptr }
	{
	}
};
}
