#pragma once

#include <Usagi/Core/Element.hpp>
#include <Usagi/Core/Math.hpp>

#include <MoeLoop/Script/LuaForwardDecl.hpp>

namespace usagi
{
struct AnimationComponent;
struct TransformComponent;
}

namespace usagi::moeloop
{
class Expression;
struct SpriteComponent;

class Character : public Element
{
    bool mShowName = true;
    bool mShowAvatar = true;

    TransformComponent *mTransform = nullptr;
    SpriteComponent *mSprite = nullptr;
    AnimationComponent *mAnimation = nullptr;
    Expression *mCurrentExpression = nullptr;

public:
    // todo load json config file
    Character(Element *parent, std::string name);

    void showName(bool show);
    void showAvatar(bool show);

    void enterScene(Expression *expr, const Vector3f &position);
    void exitScene();
    void changeExpression(Expression *expr);
    void move(const Vector3f &position);
    void say(const std::string &text);
    void pretendSay(const std::string &fake_name, const std::string &text);

    static void exportScript(kaguya::State &vm);
};
}
