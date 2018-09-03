#pragma once

#include <Usagi/Core/Element.hpp>
#include <Usagi/Core/Math.hpp>

#include <MoeLoop/Script/LuaForwardDecl.hpp>

namespace usagi
{
class Runtime;
class AssetRoot;
}

namespace usagi::moeloop
{
struct SceneConfig;
class Expression;
class Character;
class ImageLayer;

class Scene : public Element
{
    Runtime *mRuntime = nullptr;
    AssetRoot *mAsset = nullptr;
    Element *mCharacters = nullptr;
    Element *mExpressions = nullptr;
    Element *mImageLayers = nullptr;
    std::shared_ptr<SceneConfig> mConfig;

public:
    Scene(
        Element *parent,
        std::string asset_locator,
        Runtime *runtime,
        AssetRoot *asset);

    void load();

    ImageLayer * createImageLayer(const std::string &name, float y_pos);
    Character * loadCharacter(const std::string &asset_locator);
    Expression * loadExpression(const std::string &name);
    Vector3f getPosition(const std::string &name) const;

    Runtime * runtime() const { return mRuntime; }
    AssetRoot * asset() const { return mAsset; }

    static void exportScript(kaguya::State &vm);
};
}
