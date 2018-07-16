#include <gtest/gtest.h>

#include <Usagi/Engine/Asset/Filesystem/FilesystemAssetPackage.hpp>
#include <Usagi/Engine/Asset/Asset.hpp>
#include <Usagi/Engine/Graphics/Shader/SpirvBinary.hpp>
#include <Usagi/Engine/Game/Game.hpp>
#include <Usagi/Engine/Asset/AssetRoot.hpp>

using namespace usagi;

TEST(FilesystemAssetPackageTest, LoadTest)
{
    FilesystemAssetPackage pkg { nullptr, "FsPackage", "data/shaders" };
    Asset *asset = nullptr;

    EXPECT_NO_THROW(asset = pkg.findByString("glsl_shader.vert"));
    EXPECT_NO_THROW(asset->load());
    EXPECT_NO_THROW(asset->as<SpirvBinary>());
    EXPECT_EQ(asset->assetType(), AssetType::VERTEX_SHADER);

    EXPECT_NO_THROW(asset = pkg.findByString("glsl_shader.frag"));
    EXPECT_NO_THROW(asset->load());
    EXPECT_NO_THROW(asset->as<SpirvBinary>());
    EXPECT_EQ(asset->assetType(), AssetType::FRAGMENT_SHADER);
}

TEST(AssetRootTest, LoadTest)
{
    Game game;
    auto asset_root = game.assets();
    auto pkg = asset_root->addChild<FilesystemAssetPackage>(
        "FsPackage", "data/shaders"
    );
    std::shared_ptr<SpirvBinary> vert1, vert2;
    EXPECT_NO_THROW(vert1 = asset_root->find<SpirvBinary>(
        "glsl_shader.vert"));
    EXPECT_NO_THROW(vert2 = asset_root->find<SpirvBinary>(
        "FsPackage:glsl_shader.vert"));
    // no double-loading
    EXPECT_EQ(vert1.get(), vert2.get());
}
