#ifdef _WIN32

#include <Usagi/Engine/Game/Game.hpp>

#include <Usagi/Engine/Extension/Win32/Win32Platform.hpp>

void usagi::Game::initializePlatform()
{
    mPlatform = std::make_shared<Win32Platform>();
}

#endif
