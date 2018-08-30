#pragma once

#include <Usagi/Core/Element.hpp>
#include <Usagi/Core/Math.hpp>

#include <MoeLoop/Script/LuaForwardDecl.hpp>

namespace usagi::moeloop
{
class Expression;
class Character;
class ImageLayer;

class Scene : public Element
{
    Vector2u32 mSize;

public:
    Scene(
        Element *parent,
        std::string name,
        std::uint32_t width,
        std::uint32_t height);

    ImageLayer * createImageLayer(const std::string &name, float y_pos);
    Character * createCharacter(const std::string &name);
    std::shared_ptr<Expression> createExpression(const std::string &name);
    Vector3f createPosition(const std::string &name, float x, float y, float z);

    static void exportScript(kaguya::State &vm);
};
}
