#include "Load.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Converter/GpuImageAssetConverter.hpp>
#include <Usagi/Game/Game.hpp>
#include <Usagi/Runtime/Runtime.hpp>

std::shared_ptr<usagi::GpuImage> usagi::loadTexture(
    Game *game,
    const std::string &locator)
{
    return game->assets()->res<GpuImageAssetConverter>(
        locator, game->runtime()->gpu()
    );
}
