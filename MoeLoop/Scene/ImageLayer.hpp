#pragma once

#include <Usagi/Core/Element.hpp>

#include <MoeLoop/Script/LuaForwardDecl.hpp>

namespace usagi::moeloop
{
class ImageLayer : public Element
{
public:
    ImageLayer(Element *parent, std::string name, float y_pos);

    void changeImage(const std::string &name);

    static void exportScript(kaguya::State &vm);
};
}
