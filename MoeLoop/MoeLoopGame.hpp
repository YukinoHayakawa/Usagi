#pragma once

#include <Usagi/Graphics/Game/GraphicalGame.hpp>
#include <Usagi/Interactive/InputMapping.hpp>

#include "Script/Lua.hpp"

namespace usagi
{
class GameState;
}

namespace usagi::moeloop
{
class MoeLoopGame : public GraphicalGame
{
protected:
    kaguya::State mLuaContext;

    InputMapping mInputMapping;

    void bindScript();

    static void unimplemented(const std::string &msg);
    void addFilesystemPackage(std::string name, const std::string &path);

    virtual GameState * creteState(const std::string &name) = 0;
    void changeState(const std::string &name);
    void pushState(const std::string &name);
    void popState();

public:
    explicit MoeLoopGame(std::shared_ptr<Runtime> runtime);
    ~MoeLoopGame();

    /**
     * \brief Push InitGameState and execute init script.
     */
    virtual void init();

    kaguya::State & luaContext() { return mLuaContext; }
};
}
