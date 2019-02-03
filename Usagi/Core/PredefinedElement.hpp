#pragma once

#include <tuple>

#include "Element.hpp"

namespace usagi
{
// todo: need fix - component can be added/removed during runtime. if they are fixed in construction, bad things could happen when trying to remove a component. prevent removing builtin component, or forbid builtin component all together
template <typename... BuiltInComponents>
class PredefinedElement : public Element
{
protected:
    std::tuple<BuiltInComponents*...> mBuiltInComponents;

public:
    PredefinedElement(Element *parent, std::string name)
        : Element(parent, std::move(name))
    {
        mBuiltInComponents = {
            Element::addComponent<BuiltInComponents>()...
        };
    }

    template <typename Comp>
    Comp * comp()
    {
        return std::get<Comp*>(mBuiltInComponents);
    }
};
}
