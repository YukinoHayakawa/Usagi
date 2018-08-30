#include "MoeLoop.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Filesystem/FilesystemAssetPackage.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Window/Window.hpp>
#include <Usagi/Runtime/Window/WindowManager.hpp>
#include <Usagi/Runtime/Input/InputManager.hpp>
#include <Usagi/Runtime/Input/Mouse/Mouse.hpp>

#include "Scene/Scene.hpp"
#include "Scene/Character.hpp"
#include "Scene/ImageLayer.hpp"

namespace usagi::moeloop
{
MoeLoop::MoeLoop(Runtime *runtime)
    : SingleWindowGame(runtime, u8"MoeLoop", { 100, 100 }, { 1920, 1080 })
{
    assets()->addChild<FilesystemAssetPackage>(
        runtime, "moeloop", "Data/moeloop");

    //auto tex = assets()->find<GpuImage>("moeloop:images/test.png");

    runtime->inputManager()->virtualMouse()->addEventListener(this);
}

MoeLoop::~MoeLoop()
{
    mRuntime->inputManager()->virtualMouse()->removeEventListener(this);
}

void MoeLoop::mainLoop()
{
    bindScript();

    mCurrentScript = mLuaContext.newThread();
    auto scene_function = mLuaContext.loadfile("init.lua");

    mCurrentScript(scene_function);
    while(mWindow->isOpen())
    {
        mRuntime->windowManager()->processEvents();
        mRuntime->inputManager()->processEvents();
    }
}

void MoeLoop::onMouseButtonStateChange(const MouseButtonEvent &e)
{
    if(e.button == MouseButtonCode::LEFT && e.pressed)
        continueScript();
}

void MoeLoop::onMouseWheelScroll(const MouseWheelEvent &e)
{
    if(e.distance.y() < 0)
        continueScript();
}

void MoeLoop::bindScript()
{
    using namespace std::placeholders;

    mLuaContext["unimplemented"].setFunction(&MoeLoop::unimplemented);

    mLuaContext["MoeLoop"].setClass(kaguya::UserdataMetatable<MoeLoop>()
        .addFunction("createScene", &MoeLoop::createScene)
    );
    mLuaContext["ml"] = this;

    Scene::exportScript(mLuaContext);
    Character::exportScript(mLuaContext);
    ImageLayer::exportScript(mLuaContext);
}

Scene * MoeLoop::createScene(
    const std::string &name,
    std::uint32_t width,
    std::uint32_t height)
{
    return rootElement()->addChild<Scene>(name, width, height);
}

void MoeLoop::unimplemented(const std::string &msg)
{
    LOG(warn, "Script: unimplemented code: {}", msg);
}

void MoeLoop::continueScript()
{
    mCurrentScript.resume<std::string>();
}
}
