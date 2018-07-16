#ifdef _WIN32

#include "Game.hpp"

#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>

void usagi::Game::initializeDevices()
{
    // todo: window params
    const auto window = std::make_shared<Win32Window>(
        "Usagi",
        Vector2u32 { 1920, 1080 }
    );
    mWindow = window;
    mKeyboard = window;
    mMouse = window;

    // setup event listeners
    mKeyboard->addEventListener(this);
    mMouse->addEventListener(this);
}

#endif
