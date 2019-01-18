#include "InputMapping.hpp"

#include <Usagi/Utility/TypeCast.hpp>
#include <Usagi/Core/Logging.hpp>

bool usagi::InputMapping::acceptChild(Element *child)
{
    return is_instance_of<ActionGroup>(child);
}

usagi::InputMapping::InputMapping(Element *parent, std::string name)
    : Element(parent, std::move(name))
{
}

void usagi::InputMapping::disableAllActionGroups()
{
    LOG(info, "Disabling all action groups.");
    forEachChild<ActionGroup>([](ActionGroup *g) {
        g->deactivate();
    });
}

bool usagi::InputMapping::onMouseMove(const MousePositionEvent &e)
{
    for(auto i = mChildren.rbegin(); i != mChildren.rend(); ++i)
    {
        const auto g = static_cast<ActionGroup*>(i->get());
        if(g->isActive() && g->onMouseMove(e))
            return true;
    }
    return false;
}

bool usagi::InputMapping::onKeyStateChange(const KeyEvent &e)
{
    for(auto i = mChildren.rbegin(); i != mChildren.rend(); ++i)
    {
        const auto g = static_cast<ActionGroup*>(i->get());
        if(g->isActive() && g->onKeyStateChange(e))
            return true;
    }
    return false;
}

bool usagi::InputMapping::onMouseButtonStateChange(const MouseButtonEvent &e)
{
    for(auto i = mChildren.rbegin(); i != mChildren.rend(); ++i)
    {
        const auto g = static_cast<ActionGroup*>(i->get());
        if(g->isActive() && g->onMouseButtonStateChange(e))
            return true;
    }
    return false;
}
