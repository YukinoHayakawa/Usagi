#pragma once

#include <memory>
#include <vector>
#include <functional>

#include <Usagi/Core/Clock.hpp>
#include <Usagi/Core/Element.hpp>
#include <Usagi/Utility/Noncopyable.hpp>

namespace usagi
{
class GameStateManager;
class AssetRoot;
class Runtime;

class Game : Noncopyable
{
protected:
    // Multiple Game may use the same runtime & asset provider
    std::shared_ptr<Runtime> mRuntime;
    Element mRootElement { nullptr };
    AssetRoot *mAssetRoot = nullptr;
    GameStateManager *mStateManager = nullptr;
    using DeferredAction = std::function<void()>;
    std::vector<DeferredAction> mDeferredActions;

    Clock mMasterClock;

    virtual bool continueGame() const { return true; }
    void processInput();
    void updateClock();
    void performDeferredActions();
    virtual void frame();

public:
    explicit Game(std::shared_ptr<Runtime> runtime);
    virtual ~Game() = default;

    void addDeferredAction(DeferredAction action);

    Runtime * runtime() const { return mRuntime.get(); }
    AssetRoot * assets() const { return mAssetRoot; }
    GameStateManager * states() const { return mStateManager; }

    virtual void mainLoop();
};
}
