#pragma once

#include <Usagi/Core/Element.hpp>
#include <Usagi/Core/Math.hpp>

#include <MoeLoop/Script/LuaForwardDecl.hpp>
#include <MoeLoop/JSON/JSONForwardDecl.hpp>

namespace usagi
{
class Runtime;
class AssetRoot;
}

namespace usagi::moeloop
{
class Expression;
class Character;
class ImageLayer;

class Scene : public Element
{
    Runtime *mRuntime = nullptr;
    AssetRoot *mAssets = nullptr;
    Element *mCharacters = nullptr;
    Element *mExpressions = nullptr;
    Element *mImageLayers = nullptr;

    AlignedBox3f mBound;
    std::map<std::string, Vector3f> mPositions;

public:
    Scene(
        Element *parent,
        std::string name,
        Runtime *runtime,
        AssetRoot *asset);

    void load(const json &j);

    ImageLayer * createImageLayer(const std::string &name, float y_pos);
    Character * loadCharacter(const std::string &asset_locator);
    Expression * loadExpression(const std::string &name);
    Vector3f getPosition(const std::string &name) const;

    Runtime * runtime() const { return mRuntime; }
    AssetRoot * asset() const { return mAssets; }

    static void exportScript(kaguya::State &vm);
};
}
