#pragma once

#include <memory>
#include <vector>
#include <chrono>

#include <Usagi/Engine/Utility/Noncopyable.hpp>
#include <Usagi/Engine/Core/Element.hpp>

namespace usagi
{
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

class Game : Noncopyable
{
    std::unique_ptr<Window> mWindow;
    std::unique_ptr<GpuDevice> mGpuDevice;
    std::vector<SubsystemInfo> mSubsystems;
    Element mRootElement { nullptr };
    AssetRoot *mAssetRoot = nullptr;

    std::vector<SubsystemInfo>::iterator findSubsystemByName(
        const std::string &subsystem_name);

    void setSubsystemEnabled(
        const std::string &subsystem_name,
        bool enabled);

public:
    Game();
    ~Game();

	Window * window() const { return mWindow.get(); }
	GpuDevice * gpuDevice() const { return mGpuDevice.get(); }

    /**
     * \brief
     * \param subsystem 
     */
    Subsystem * addSubsystem(SubsystemInfo subsystem);

    void enableSubsystem(const std::string &subsystem_name);
    void disableSubsystem(const std::string &subsystem_name);

    Element * rootElement() { return &mRootElement; }
    Asset * assets() const;

    /**
    * \brief Invoke update methods on each enabled subsystem by the order
    * of their registration.
    * \param dt The elapsed time from last frame.
    */
    void update(const std::chrono::seconds &dt);
};
}
