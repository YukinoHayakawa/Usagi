#include "DebugDrawDemo.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Package/Filesystem/FilesystemAssetPackage.hpp>
#include <Usagi/Game/GameStateManager.hpp>
#include <Usagi/Runtime/Window/Window.hpp>

#include "DebugDrawGameState.hpp"

usagi::DebugDrawDemo::DebugDrawDemo(std::shared_ptr<Runtime> runtime)
    : GraphicalGame(std::move(runtime))
{
    assets()->addChild<FilesystemAssetPackage>("dd", "Data/debugdraw");
    createMainWindow(
        u8"🐰 - DebugDraw Demo",
        Vector2i { 100, 100 },
        Vector2u32 { 1920, 1080 }
    );
    setupRenderTargets(true);

    mStateManager->pushState(mStateManager->addChild<DebugDrawGameState>(
        "DebugDraw", this));
}
