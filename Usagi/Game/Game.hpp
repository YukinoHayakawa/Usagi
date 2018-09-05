#pragma once

#include <memory>
#include <vector>

#include <Usagi/Core/Element.hpp>
#include <Usagi/Utility/Noncopyable.hpp>
#include <Usagi/Utility/TypeCast.hpp>

namespace usagi
{
class Clock;
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
protected:
    // Multiple Game may use the same runtime.
    Runtime *mRuntime = nullptr;
    std::vector<SubsystemInfo> mSubsystems;
    Element mRootElement { nullptr };
    AssetRoot *mAssetRoot = nullptr;

    std::vector<SubsystemInfo>::iterator findSubsystemByName(
        const std::string &subsystem_name);

    void setSubsystemEnabled(
        const std::string &subsystem_name,
        bool enabled);

    Subsystem * addSubsystemPtr(
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
        auto ptr = subsystem.get();
        addSubsystemPtr(
            std::move(name),
            unique_pointer_cast<Subsystem>(std::move(subsystem))
        );
        return ptr;
    }

    void enableSubsystem(const std::string &subsystem_name);
    void disableSubsystem(const std::string &subsystem_name);

    Runtime * runtime() const { return mRuntime; }
    Element * rootElement() { return &mRootElement; }
    AssetRoot * assets() const { return mAssetRoot; }

    /**
    * \brief Invoke update methods on each enabled subsystem by the order
    * of their registration.
    * \param clock The elapsed time from last frame.
    */
    void update(const Clock &clock);
};
}
