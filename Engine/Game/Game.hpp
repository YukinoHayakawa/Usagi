#pragma once

#include <memory>
#include <vector>
#include <chrono>

#include <Usagi/Engine/Utility/Noncopyable.hpp>
#include <Usagi/Engine/Core/Element.hpp>
#include <Usagi/Engine/Runtime/HID/Mouse/MouseEventListener.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/KeyEventListener.hpp>

namespace usagi
{
class Platform;
class Keyboard;
class Mouse;
class Asset;
class AssetRoot;
class Subsystem;
class Window;
class GpuDevice;

struct SubsystemInfo
{
    std::string name;
    std::unique_ptr<Subsystem> subsystem;
    bool enabled = true;
};

class Game
    : Noncopyable
    , public KeyEventListener
    , public MouseEventListener
{
    std::shared_ptr<Platform> mPlatform;
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Keyboard> mKeyboard;
    std::shared_ptr<Mouse> mMouse;
    std::unique_ptr<GpuDevice> mGpuDevice;

    std::vector<SubsystemInfo> mSubsystems;
    Element mRootElement { nullptr };
    AssetRoot *mAssetRoot = nullptr;

    std::vector<SubsystemInfo>::iterator findSubsystemByName(
        const std::string &subsystem_name);

    void setSubsystemEnabled(
        const std::string &subsystem_name,
        bool enabled);

    void onMouseMove(const MousePositionEvent &e) override;
    void onMouseButtonStateChange(const MouseButtonEvent &e) override;
    void onMouseWheelScroll(const MouseWheelEvent &e) override;
    void onKeyStateChange(const KeyEvent &e) override;

    void initializePlatform();

public:
    Game();
    ~Game();

    void initializeGraphics();

    Platform * platform() const { return mPlatform.get(); }
	GpuDevice * gpuDevice() const { return mGpuDevice.get(); }

    /**
     * \brief
     * \param subsystem 
     */
    Subsystem * addSubsystem(SubsystemInfo subsystem);

    void enableSubsystem(const std::string &subsystem_name);
    void disableSubsystem(const std::string &subsystem_name);

    Element * rootElement() { return &mRootElement; }
    AssetRoot * assets() const { return mAssetRoot; }

    /**
    * \brief Invoke update methods on each enabled subsystem by the order
    * of their registration.
    * \param dt The elapsed time from last frame.
    */
    void update(const std::chrono::seconds &dt);
};
}
