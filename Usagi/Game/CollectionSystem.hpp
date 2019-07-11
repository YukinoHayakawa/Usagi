#pragma once

#include <tuple>
#include <map>

#include <Usagi/Core/Element.hpp>
#include <Usagi/Core/Logging.hpp>

#include "System.hpp"

namespace usagi
{
template <typename... RequiredComponents>
class CollectionSystem : virtual public System
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
            LOG(debug, "Registering\n"
                "    Element {}: {} at\n"
                "    System  {}: {}",
                static_cast<void*>(element), element->path(),
                static_cast<void*>(this), typeid(*this).name()
            );
            mRegistry.insert_or_assign(element, std::move(vec));
        }
        else
        {
            const bool erased = mRegistry.erase(element);
            if(erased)
            {
                LOG(debug, "Removed\n"
                    "    Element {}: {} from\n"
                    "    System  {}: {}",
                    static_cast<void *>(element), element->path(),
                    static_cast<void *>(this), typeid(*this).name()
                );
            }
        }
    }
};
}
