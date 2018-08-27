#pragma once

#include "ChildElementEvent.hpp"

namespace usagi
{
class ChildElementAddedEvent : public ChildElementEvent
{
public:
	explicit ChildElementAddedEvent(Element *child)
		: ChildElementEvent { child }
	{
	}
};
}
