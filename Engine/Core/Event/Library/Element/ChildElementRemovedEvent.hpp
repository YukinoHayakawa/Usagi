#pragma once

#include "ChildElementEvent.hpp"

namespace usagi
{
class ChildElementRemovedEvent : public ChildElementEvent
{
public:
	explicit ChildElementRemovedEvent()
		: ChildElementEvent { nullptr }
	{
	}
};
}
