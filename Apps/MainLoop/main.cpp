#include <Usagi/Engine/Core/Logging.hpp>
#include <Usagi/Engine/Game/Game.hpp>
#include <Usagi/Engine/Runtime/Platform.hpp>
#include <Usagi/Engine/Runtime/Window/Window.hpp>
#include <Usagi/Engine/Runtime/HID/Mouse/Mouse.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/Keyboard.hpp>
#include <Usagi/Engine/Extension/Win32/Win32Platform.hpp>
#include <Usagi/Engine/Extension/Win32/Win32Helper.hpp>

using namespace usagi;
using namespace std::chrono_literals;

class MainLoop
    : public KeyEventListener
    , public MouseEventListener
    , public WindowEventListener
{
    Game mGame;
    std::shared_ptr<Window> mWindow = mGame.platform()->createWindow(
        u8"\U0001F430 - Event Handling Test",
        Vector2i { 100, 100 },
        Vector2u32 { 1920, 1080 }
    );
    std::shared_ptr<Mouse> mMouse = mGame.platform()->virtualMouse();
    std::shared_ptr<Keyboard> mKeyboard = mGame.platform()->virtualKeyboard();

public:
    MainLoop()
    {
        mGame.initializeInput();
        mKeyboard->addEventListener(this);
        mWindow->addEventListener(this);
    }

    ~MainLoop()
    {
        mMouse->removeEventListener(this);
        mKeyboard->removeEventListener(this);
        mWindow->removeEventListener(this);
    }

    void onMouseMove(const MousePositionEvent &e) override
    {
        LOG(info, "Mouse moved:       {}",
            e.distance.transpose()
        );
    }

    void onMouseButtonStateChange(const MouseButtonEvent &e) override
    {
        LOG(info, "Mouse button:      {}={}",
            to_string(e.button), e.pressed
        );
    }

    void onMouseWheelScroll(const MouseWheelEvent &e) override
    {
        LOG(info, "Mouse wheel:       {}",
            e.distance.transpose()
        );
    }

    void onKeyStateChange(const KeyEvent &e) override
    {
        LOG(info, "Key:               {}={}, repeated={}",
            to_string(e.key_code), e.pressed, e.repeated
        );
        switch(e.key_code)
        {
            case KeyCode::ESCAPE:
                mWindow->close();
                break;
            case KeyCode::I:
                if(e.pressed)
                    mMouse->setImmersiveMode(
                        !mMouse->isImmersiveMode()
                    );
                break;
            // Mouse events can overwhelm the console window...
            case KeyCode::M:
                if(e.pressed)
                {
                    if(mMouse->hasEventListener(this))
                    {
                        mMouse->removeEventListener(this);
                        LOG(info, "Mouse listener removed");
                    } else
                    {
                        mMouse->addEventListener(this);
                        LOG(info, "Mouse listener added");
                    }
                }
                break;
            case KeyCode::NUM_1:
                if(e.pressed)
                    mWindow->setSize({ 640, 480 });
                break;
            case KeyCode::NUM_2:
                if(e.pressed)
                    mWindow->setSize({ 1280, 720 });
                break;
            case KeyCode::NUM_3:
                if(e.pressed)
                    mWindow->setSize({ 1920, 1080 });
                break;
            case KeyCode::NUM_4:
                if(e.pressed)
                    mWindow->setPosition({ 0, 0 });
                break;
            case KeyCode::NUM_5:
                if(e.pressed)
                    mWindow->setPosition({ 1280, 720 });
                break;
            case KeyCode::NUM_6:
                if(e.pressed)
                    mWindow->setPosition({ 1920, 1080 });
                break;
            default:
                break;
        }
    }

    void onWindowFocusChanged(const WindowFocusEvent &e) override
    {
        LOG(info, "Window focus:      {}",
            e.focused
        );
    }

    void onWindowMoveBegin(const WindowPositionEvent &e) override
    {
        LOG(info, "Window move begin");
    }

    void onWindowMoved(const WindowPositionEvent &e) override
    {
        LOG(info, "Window position:   {}",
            e.position.transpose()
        );
    }

    void onWindowMoveEnd(const WindowPositionEvent &e) override
    {
        LOG(info, "Window move end");
    }

    void onWindowResizeBegin(const WindowSizeEvent &e) override
    {
        LOG(info, "Window resize begin");
    }

    void onWindowResized(const WindowSizeEvent &e) override
    {
        LOG(info, "Window size:       {}",
            e.size.transpose()
        );
    }

    void onWindowResizeEnd(const WindowSizeEvent &e) override
    {
        LOG(info, "Window resize end");
    }

    void mainLoop()
    {
        while(mWindow->isOpen())
        {
            mGame.update(0s);
        }
    }
};

int main(int argc, char *argv[])
{
    win32::patchConsole();
    //Win32Platform::updateDeviceNames();
    MainLoop app;
    app.mainLoop();
}
