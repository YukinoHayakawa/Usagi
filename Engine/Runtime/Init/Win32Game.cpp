#ifdef _WIN32

#include <Usagi/Engine/Game/Game.hpp>

#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>

void usagi::Game::initializeInput()
{
    // todo: window params
    const auto window = std::make_shared<Win32Window>(
        "Usagi",
        Vector2i { 100, 100 },
        Vector2u32 { 1920, 1080 }
    );
    mWindow = window;
    // dummy deleters are used because the mouse and keyboard are managed
    // by the window
    mKeyboard = std::shared_ptr<Keyboard> { window->keyboard(), [](auto) { } };
    mMouse = std::shared_ptr<Mouse> { window->mouse(), [](auto) { } };

    // setup event listeners
    mKeyboard->addEventListener(this);
    mMouse->addEventListener(this);
}

#endif
