#include "Win32Keyboard.hpp"

#include <Usagi/Runtime/Input/Keyboard/KeyCode.hpp>
#include <Usagi/Runtime/Input/Keyboard/KeyEventListener.hpp>

usagi::KeyCode usagi::Win32Keyboard::translate(const RAWKEYBOARD *keyboard)
{
    switch(keyboard->VKey)
    {
        case VK_BACK:
            return KeyCode::BACKSPACE;
        case VK_TAB:
            return KeyCode::TAB;
        case VK_PAUSE:
            return KeyCode::PAUSE;
        case VK_CAPITAL:
            return KeyCode::CAPSLOCK;
        case VK_ESCAPE:
            return KeyCode::ESCAPE;
        case VK_SPACE:
            return KeyCode::SPACE;
        case VK_PRIOR:
            return KeyCode::PAGE_UP;
        case VK_NEXT:
            return KeyCode::PAGE_DOWN;
        case VK_END:
            return KeyCode::END;
        case VK_HOME:
            return KeyCode::HOME;
        case VK_LEFT:
            return KeyCode::LEFT;
        case VK_UP:
            return KeyCode::UP;
        case VK_RIGHT:
            return KeyCode::RIGHT;
        case VK_DOWN:
            return KeyCode::DOWN;
        case VK_SNAPSHOT:
            return KeyCode::PRINT_SCREEN;
        case VK_INSERT:
            return KeyCode::INSERT;
        case VK_DELETE:
            return KeyCode::DELETE;
        case '0':
            return KeyCode::DIGIT_0;
        case '1':
            return KeyCode::DIGIT_1;
        case '2':
            return KeyCode::DIGIT_2;
        case '3':
            return KeyCode::DIGIT_3;
        case '4':
            return KeyCode::DIGIT_4;
        case '5':
            return KeyCode::DIGIT_5;
        case '6':
            return KeyCode::DIGIT_6;
        case '7':
            return KeyCode::DIGIT_7;
        case '8':
            return KeyCode::DIGIT_8;
        case '9':
            return KeyCode::DIGIT_9;
        case 'A':
            return KeyCode::A;
        case 'B':
            return KeyCode::B;
        case 'C':
            return KeyCode::C;
        case 'D':
            return KeyCode::D;
        case 'E':
            return KeyCode::E;
        case 'F':
            return KeyCode::F;
        case 'G':
            return KeyCode::G;
        case 'H':
            return KeyCode::H;
        case 'I':
            return KeyCode::I;
        case 'J':
            return KeyCode::J;
        case 'K':
            return KeyCode::K;
        case 'L':
            return KeyCode::L;
        case 'M':
            return KeyCode::M;
        case 'N':
            return KeyCode::N;
        case 'O':
            return KeyCode::O;
        case 'P':
            return KeyCode::P;
        case 'Q':
            return KeyCode::Q;
        case 'R':
            return KeyCode::R;
        case 'S':
            return KeyCode::S;
        case 'T':
            return KeyCode::T;
        case 'U':
            return KeyCode::U;
        case 'V':
            return KeyCode::V;
        case 'W':
            return KeyCode::W;
        case 'X':
            return KeyCode::X;
        case 'Y':
            return KeyCode::Y;
        case 'Z':
            return KeyCode::Z;
        case VK_LWIN:
            return KeyCode::LEFT_OS;
        case VK_RWIN:
            return KeyCode::RIGHT_OS;
        case VK_APPS:
            return KeyCode::CONTEXT_MENU;
        case VK_NUMPAD0:
            return KeyCode::NUM_0;
        case VK_NUMPAD1:
            return KeyCode::NUM_1;
        case VK_NUMPAD2:
            return KeyCode::NUM_2;
        case VK_NUMPAD3:
            return KeyCode::NUM_3;
        case VK_NUMPAD4:
            return KeyCode::NUM_4;
        case VK_NUMPAD5:
            return KeyCode::NUM_5;
        case VK_NUMPAD6:
            return KeyCode::NUM_6;
        case VK_NUMPAD7:
            return KeyCode::NUM_7;
        case VK_NUMPAD8:
            return KeyCode::NUM_8;
        case VK_NUMPAD9:
            return KeyCode::NUM_9;
        case VK_MULTIPLY:
            return KeyCode::NUM_MULTIPLY;
        case VK_ADD:
            return KeyCode::NUM_ADD;
        case VK_SUBTRACT:
            return KeyCode::NUM_SUBTRACT;
        case VK_DECIMAL:
            return KeyCode::NUM_DECIMAL;
        case VK_DIVIDE:
            return KeyCode::NUM_DIVIDE;
        case VK_F1:
            return KeyCode::F1;
        case VK_F2:
            return KeyCode::F2;
        case VK_F3:
            return KeyCode::F3;
        case VK_F4:
            return KeyCode::F4;
        case VK_F5:
            return KeyCode::F5;
        case VK_F6:
            return KeyCode::F6;
        case VK_F7:
            return KeyCode::F7;
        case VK_F8:
            return KeyCode::F8;
        case VK_F9:
            return KeyCode::F9;
        case VK_F10:
            return KeyCode::F10;
        case VK_F11:
            return KeyCode::F11;
        case VK_F12:
            return KeyCode::F12;
        case VK_NUMLOCK:
            return KeyCode::NUM_LOCK;
        case VK_SCROLL:
            return KeyCode::SCROLL_LOCK;
        case VK_OEM_1:
            return KeyCode::SEMICOLON;
        case VK_OEM_PLUS:
            return KeyCode::EQUAL;
        case VK_OEM_COMMA:
            return KeyCode::COMMA;
        case VK_OEM_MINUS:
            return KeyCode::MINUS;
        case VK_OEM_PERIOD:
            return KeyCode::PERIOD;
        case VK_OEM_2:
            return KeyCode::SLASH;
        case VK_OEM_3:
            return KeyCode::BACKQUOTE;
        case VK_OEM_4:
            return KeyCode::LEFT_BRACKET;
        case VK_OEM_5:
            return KeyCode::BACKSLASH;
        case VK_OEM_6:
            return KeyCode::RIGHT_BRACKET;
        case VK_OEM_7:
            return KeyCode::QUOTE;
        default:
            break;
    }
    const auto e0_prefixed = (keyboard->Flags & RI_KEY_E0) != 0;
    switch(keyboard->VKey)
    {
        case VK_SHIFT:
            return MapVirtualKey(keyboard->MakeCode, MAPVK_VSC_TO_VK_EX)
                == VK_LSHIFT ? KeyCode::LEFT_SHIFT : KeyCode::RIGHT_SHIFT;
        case VK_CONTROL:
            return e0_prefixed ? KeyCode::RIGHT_CONTROL : KeyCode::LEFT_CONTROL;
        case VK_MENU:
            return e0_prefixed ? KeyCode::RIGHT_ALT : KeyCode::LEFT_ALT;
        case VK_RETURN:
            return e0_prefixed ? KeyCode::NUM_ENTER : KeyCode::ENTER;
        default:
            return KeyCode::UNKNOWN;
    }
}

void usagi::Win32Keyboard::sendKeyEvent(
    const KeyCode key,
    const bool pressed,
    const bool repeated)
{
    KeyEvent e;
    e.keyboard = this;
    e.key_code = key;
    e.pressed = pressed;
    e.repeated = repeated;
    forEachListener([&](auto h) {
        h->onKeyStateChange(e);
    });
}

void usagi::Win32Keyboard::handleRawInput(RAWINPUT *raw)
{
    checkDevice(raw, RIM_TYPEKEYBOARD);

    // todo
    //if(!Win32Platform::activeWindow()) return;

    auto &kb = raw->data.keyboard;

    const auto key = translate(&kb);
    // ignore keys other than those on 101 keyboard
    if(key == KeyCode::UNKNOWN)
        return;

    const auto pressed = (kb.Flags & RI_KEY_BREAK) == 0;
    auto repeated = false;

    if(pressed)
        if(mKeyPressed.count(key) != 0)
            repeated = true;
        else
            mKeyPressed.insert(key);
    else
        mKeyPressed.erase(key);

    sendKeyEvent(key, pressed, repeated);
}

void usagi::Win32Keyboard::clearKeyPressedStates()
{
    for(auto iter = mKeyPressed.begin(); iter != mKeyPressed.end();)
    {
        sendKeyEvent(*iter, false, false);
        iter = mKeyPressed.erase(iter);
    }
}

void usagi::Win32Keyboard::onWindowFocusChanged(const WindowFocusEvent &e)
{
    if(!e.focused)
        clearKeyPressedStates();
}

usagi::Win32Keyboard::Win32Keyboard(HANDLE device_handle, std::string name)
    : Win32RawInputDevice { device_handle, std::move(name) }
{
}

std::string usagi::Win32Keyboard::name() const
{
    return mName;
}

bool usagi::Win32Keyboard::isKeyPressed(KeyCode key) const
{
    return mKeyPressed.count(key) != 0;
}
