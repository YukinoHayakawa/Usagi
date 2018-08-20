#pragma once

#include <memory>
#include <vector>

#include <Usagi/Engine/Utility/Noncopyable.hpp>
#include <Usagi/Engine/Core/Element.hpp>
#include <Usagi/Engine/Utility/TypeCast.hpp>
#include <Usagi/Engine/Core/Time.hpp>

namespace usagi
{
class Runtime;
class Subsystem;
class AssetRoot;

struct SubsystemInfo
{
    std::string name;
    std::unique_ptr<Subsystem> subsystem;
    bool enabled = true;
};

class Game : Noncopyable
{
    // Multiple Game may use the same runtime.
    Runtime* mRuntime = nullptr;
    std::vector<SubsystemInfo> mSubsystems;
    Element mRootElement { nullptr };
    AssetRoot* mAssetRoot = nullptr;

    std::vector<SubsystemInfo>::iterator findSubsystemByName(
        const std::string& subsystem_name);

    void setSubsystemEnabled(
        const std::string& subsystem_name,
        bool enabled);

    Subsystem* addSubsystemPtr(
        std::string name,
        std::unique_ptr<Subsystem> subsystem);

public:
    explicit Game(Runtime *runtime);
    ~Game();

    // moving will change AssetRoot address.
    Game & operator=(const Game &other) = delete;
    Game & operator=(Game &&other) noexcept = delete;

    template <typename SubsystemT>
    SubsystemT * addSubsystem(
        std::string name,
        std::unique_ptr<SubsystemT> subsystem)
    {
        return static_cast<SubsystemT*>(addSubsystemPtr(
            std::move(name),
            unique_pointer_cast<Subsystem>(std::move(subsystem))
        ));
    }

    void enableSubsystem(const std::string& subsystem_name);
    void disableSubsystem(const std::string& subsystem_name);

    Runtime * runtime() const { return mRuntime; }
    Element * rootElement() { return &mRootElement; }
    AssetRoot * assets() const { return mAssetRoot; }

    /**
    * \brief Invoke update methods on each enabled subsystem by the order
    * of their registration.
    * \param dt The elapsed time from last frame.
    */
    void update(const TimeDuration& dt);
};
}
