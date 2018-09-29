#include <chrono>

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Window/Window.hpp>
#include <Usagi/Runtime/Window/WindowManager.hpp>
#include <Usagi/Runtime/Input/Mouse/Mouse.hpp>
#include <Usagi/Runtime/Input/Mouse/MouseEventListener.hpp>
#include <Usagi/Runtime/Input/Keyboard/Keyboard.hpp>
#include <Usagi/Runtime/Input/Keyboard/KeyEventListener.hpp>
#include <Usagi/Runtime/Input/Gamepad/Gamepad.hpp>
#include <Usagi/Runtime/Input/Gamepad/GamepadEventListener.hpp>
#include <Usagi/Runtime/Input/InputManager.hpp>

using namespace usagi;
using namespace std::chrono_literals;

class MainLoop
    : public KeyEventListener
    , public MouseEventListener
    , public WindowEventListener
    , public GamepadEventListener
{
    std::shared_ptr<Runtime> mRuntime;
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Mouse> mMouse;
    std::shared_ptr<Keyboard> mKeyboard;
    std::shared_ptr<Gamepad> mGamepad;

public:
    MainLoop()
    {
        mRuntime = Runtime::create();

        mRuntime->initWindow();
        mRuntime->initInput();

        mWindow = mRuntime->windowManager()->createWindow(
            u8"\U0001F430 - Event Handling Test",
            Vector2i { 100, 100 },
            Vector2u32 { 1920, 1080 }
        );
        mWindow->addEventListener(this);

        const auto input_manager = mRuntime->inputManager();
        mMouse = input_manager->virtualMouse();
        mKeyboard = input_manager->virtualKeyboard();
        mKeyboard->addEventListener(this);

        const auto gamepads = input_manager->gamepads();
        if(!gamepads.empty())
        {
            mGamepad = gamepads.front();
            // todo not removed
            mGamepad->addEventListener(this);
        }
    }

    ~MainLoop()
    {
        mMouse->removeEventListener(this);
        mKeyboard->removeEventListener(this);
        mWindow->removeEventListener(this);
    }

    void onMouseMove(const MousePositionEvent &e) override
    {
        LOG(info, "Mouse moved:       abs={}, rel={}",
            e.position.transpose(),
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
                    }
                    else
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

    void onButtonStateChange(const GamepadButtonEvent &e) override
    {
        LOG(info, "Gamepad button:    {}={}",
            to_string(e.code), e.pressed
        );
    }

    void mainLoop()
    {
        while(mWindow->isOpen())
        {
            mRuntime->windowManager()->processEvents();
            mRuntime->inputManager()->processEvents();
        }
    }
};

int main(int argc, char *argv[])
{
    MainLoop app;
    app.mainLoop();
}
