#pragma once

#include <Usagi/Core/Element.hpp>

#include <MoeLoop/Script/LuaForwardDecl.hpp>

#include "TransitionableImage.hpp"

namespace usagi
{
struct TransformComponent;
class AssetRoot;
}

namespace usagi::moeloop
{
class Scene;
struct SpriteComponent;

class ImageLayer : public TransitionableImage
{
    Scene *mScene = nullptr;

public:
    ImageLayer(Element *parent, std::string name, float y_pos, Scene *scene);

    // todo pass in texture by scene
    void changeImage(const std::string &name);

    static void exportScript(kaguya::State &vm);
};
}
