#include "NuklearDemo.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Package/Filesystem/FilesystemAssetPackage.hpp>
#include <Usagi/Game/GameStateManager.hpp>
#include <Usagi/Runtime/Window/Window.hpp>

#include "NuklearDemoState.hpp"

usagi::NuklearDemo::NuklearDemo(std::shared_ptr<Runtime> runtime)
    : GraphicalGame(std::move(runtime))
{
    assets()->addChild<FilesystemAssetPackage>("nuklear", "Data/nuklear");
    createMainWindow(
        u8"🐰 - Nuklear Demo",
        Vector2i { 100, 100 },
        Vector2u32 { 1920, 1080 }
    );
    setupRenderTargets(false);

    mStateManager->pushState(mStateManager->addChild<NuklearDemoState>(
        "NuklearDemo", this));
}
