#pragma once

#include <tuple>

#include "Element.hpp"

namespace usagi
{
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
