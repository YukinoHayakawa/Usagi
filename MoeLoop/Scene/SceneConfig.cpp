#include "SceneConfig.hpp"

#include <MoeLoop/JSON/JSON.hpp>
#include <MoeLoop/JSON/Math.hpp>

namespace usagi::moeloop
{
void to_json(json &j, const SceneConfig &v)
{
    j = json { };

    // todo write a template/macro for optional save
    if(!v.name.empty()) j["name"] = v.name;
    if(!v.inherit.empty()) j["inherit"] = v.inherit;
    if(!v.size.isZero()) j["size"] = v.size;
    if(!v.positions.empty()) j["positions"] = v.positions;
}

void from_json(const json &j, SceneConfig &v)
{
    v.name = j.value<decltype(v.name)>("name", {});
    v.inherit = j.value<decltype(v.inherit)>("inherit", {});
    v.size = j.value<decltype(v.size)>("size", decltype(v.size)::Zero());
    v.positions = j.value<decltype(v.positions)>("positions", {});
}
}
