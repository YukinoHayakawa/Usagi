#pragma once

#include <map>

#include <Usagi/Core/Math.hpp>

#include <MoeLoop/JSON/JSONForwardDecl.hpp>

namespace usagi::moeloop
{
struct SceneConfig
{
    std::shared_ptr<SceneConfig> parent;

    std::string name;
    std::string inherit;
    Vector2u32 size = Vector2u32::Zero();
    std::map<std::string, Vector3f> positions;
};

void to_json(json& j, const SceneConfig& v);
void from_json(const json& j, SceneConfig& v);
}
