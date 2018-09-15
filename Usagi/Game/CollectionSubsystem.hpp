#pragma once

#include <tuple>
#include <map>

#include <Usagi/Core/Element.hpp>

#include "Subsystem.hpp"

namespace usagi
{
template <typename... RequiredComponents>
class CollectionSubsystem : virtual public Subsystem
{
protected:
    using ComponentVector = std::tuple<RequiredComponents *...>;
    using Registry = std::map<Element *, ComponentVector>;
    Registry mRegistry;

public:
    void onElementComponentChanged(Element *element) override
    {
        const ComponentVector vec = {
            element->findComponent<RequiredComponents>()...
        };
        const auto processable =
            (... && (std::get<RequiredComponents*>(vec) != nullptr));
        if(processable)
        {
            mRegistry.insert_or_assign(element, std::move(vec));
        }
        else
        {
            mRegistry.erase(element);
        }
    }
};
}
